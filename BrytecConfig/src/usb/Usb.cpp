#include "Usb.h"

#include "BrytecConfigEmbedded/Deserializer/BinaryDeserializer.h"
#include <exception>
#include <iostream>

namespace Brytec {

static void usbRxThread(bool& run, serial::Serial& serial, std::mutex& rxMutex, std::vector<uint8_t>& rxData)
{
    while (run) {

        if (serial.isOpen()) {

            uint8_t buffer[64];
            uint8_t length = 0;
            try {
                length = serial.read(buffer, 64);
            } catch (std::exception& e) {
                std::cerr << "Exception: " << e.what() << std::endl;
                serial.close();
                run = false;
            }

            if (length > 0) {
                rxMutex.lock();
                rxData.insert(rxData.end(), std::begin(buffer), std::begin(buffer) + length);
                rxMutex.unlock();
            }
        }
    }
}

static void usbTxThread(bool& run, serial::Serial& serial, std::mutex& txMutex, std::deque<UsbPacket>& txPackets)
{
    while (run) {

        if (serial.isOpen()) {

            uint8_t buffer[64];
            uint8_t length;

            if (txPackets.size() > 0) {
                txMutex.lock();
                UsbPacket sendPacket = txPackets[0];
                txPackets.pop_front();
                txMutex.unlock();

                buffer[0] = PacketStart;
                buffer[1] = sendPacket.length;
                memcpy(&buffer[2], sendPacket.data, sendPacket.length);
                try {
                    length = serial.write(buffer, sendPacket.length + 2);
                } catch (std::exception& e) {
                    std::cerr << "Exception: " << e.what() << std::endl;
                    serial.close();
                    run = false;
                }

                if (length != sendPacket.length + 2) {
                    std::cout << "Didnt send all of the data";
                }
            }
        }
    }
}

Usb::~Usb()
{
    close();
}

void Usb::open(std::string port)
{
    if (!port.empty()) {
        m_serial.setPort(port);
        serial::Timeout timeout = serial::Timeout::simpleTimeout(100);
        m_serial.setTimeout(timeout);
        m_serial.setBaudrate(115200);

        try {
            m_serial.open();
        } catch (std::exception& e) {
            std::cerr << "Exception: " << e.what() << std::endl;
        }

        if (!m_runThread && m_rxThread.joinable())
            m_rxThread.join();
        if (!m_runThread && m_txThread.joinable())
            m_txThread.join();

        if (m_serial.isOpen()) {
            m_runThread = true;
            m_rxThread = std::thread(usbRxThread, std::ref(m_runThread), std::ref(m_serial),
                std::ref(m_rxMutex), std::ref(m_rxData));
            m_txThread = std::thread(usbTxThread, std::ref(m_runThread), std::ref(m_serial),
                std::ref(m_txMutex), std::ref(m_txPackets));
        }
    }
}

void Usb::close()
{
    m_runThread = false;
    if (m_serial.isOpen())
        m_serial.close();
    if (m_rxThread.joinable())
        m_rxThread.join();
    if (m_txThread.joinable())
        m_txThread.join();
}

std::vector<UsbPacket> Usb::getReceivedPackets()
{
    m_rxMutex.lock();
    std::vector<UsbPacket> packets;

    while (UsbPacket packet = getPacketFromRaw()) {
        packets.push_back(packet);
    }

    m_rxMutex.unlock();

    return packets;
}

void Usb::send(const UsbPacket& packet)
{
    m_txMutex.lock();
    m_txPackets.push_back(packet);
    m_txMutex.unlock();
}

UsbPacket Usb::getPacketFromRaw()
{
    UsbPacket packet;

    if (m_rxData.size() <= 0)
        return packet;

    auto start = std::find(m_rxData.begin(), m_rxData.end(), PacketStart);
    if (start > m_rxData.begin()) {
        m_rxData.erase(m_rxData.begin(), start);
    }

    if (m_rxData.size() >= 2)
        packet.length = m_rxData[1];
    else
        return packet;

    if (packet.length > 64) {
        // Something wrong with packet, delete it and try again
        m_rxData.erase(m_rxData.begin());
        return packet;
    }

    if (m_rxData.size() >= packet.length + 2)
        memcpy(packet.data, &m_rxData[2], packet.length);
    else
        return packet;

    m_rxData.erase(m_rxData.begin(), m_rxData.begin() + packet.length + 2);
    return packet;
}
}