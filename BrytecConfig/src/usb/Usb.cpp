#include "Usb.h"

#include "BrytecConfigEmbedded/Can/EBrytecCan.h"
#include "BrytecConfigEmbedded/Deserializer/BinaryDeserializer.h"
#include "BrytecConfigEmbedded/Usb/UsbDefs.h"
#include <chrono>
#include <exception>
#include <iostream>

namespace Brytec {

static UsbPacket getPacket(std::vector<uint8_t>& data)
{
    UsbPacket packet;

    if (data.size() <= 0)
        return packet;

    auto start = std::find(data.begin(), data.end(), PacketStart);
    if (start > data.begin()) {
        data.erase(data.begin(), start);
    }

    if (data.size() >= 2)
        packet.length = data[1];
    else
        return packet;

    if (packet.length > 64) {
        // Something wrong with packet, delete it and try again
        data.erase(data.begin());
        return packet;
    }

    if (data.size() >= packet.length + 2)
        memcpy(packet.data, &data[2], packet.length);
    else
        return packet;

    data.erase(data.begin(), data.begin() + packet.length + 2);
    return packet;
}

static void usbRxThread(bool& run, serial::Serial& serial, std::mutex& rxMutex, std::vector<uint8_t>& rxData, std::function<void(UsbPacket)> callback)
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

                while (UsbPacket packet = getPacket(rxData)) {
                    callback(packet);
                }

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
                std::ref(m_rxMutex), std::ref(m_rxData), m_receiveCallback);
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

void Usb::send(const UsbPacket& packet)
{
    m_txMutex.lock();
    m_txPackets.push_back(packet);
    m_txMutex.unlock();
}

// void Usb::sendMultiple(const std::vector<UsbPacket>& packets)
// {
//     m_multipleThread = std::thread(usbMultipleThread, std::ref(m_serial), packets);
// }

// UsbPacket Usb::getPacketFromRaw()
// {
//     UsbPacket packet;

//     if (m_rxData.size() <= 0)
//         return packet;

//     auto start = std::find(m_rxData.begin(), m_rxData.end(), PacketStart);
//     if (start > m_rxData.begin()) {
//         m_rxData.erase(m_rxData.begin(), start);
//     }

//     if (m_rxData.size() >= 2)
//         packet.length = m_rxData[1];
//     else
//         return packet;

//     if (packet.length > 64) {
//         // Something wrong with packet, delete it and try again
//         m_rxData.erase(m_rxData.begin());
//         return packet;
//     }

//     if (m_rxData.size() >= packet.length + 2)
//         memcpy(packet.data, &m_rxData[2], packet.length);
//     else
//         return packet;

//     m_rxData.erase(m_rxData.begin(), m_rxData.begin() + packet.length + 2);
//     return packet;
// }
}