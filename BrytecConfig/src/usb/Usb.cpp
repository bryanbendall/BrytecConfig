#include "Usb.h"

#include "BrytecConfigEmbedded/Deserializer/BinaryDeserializer.h"
#include <iostream>

namespace Brytec {

static void usbThread(bool& run, serial::Serial& serial, std::mutex& mutex, std::map<uint32_t, Brytec::CanExtFrame>& vec)
{
    while (run) {

        size_t readLenth;
        if (serial.isOpen()) {

            static uint8_t buffer[64];
            try {
                readLenth = serial.read(buffer, 64);
            } catch (std::exception& e) {
                std::cerr << "Exception: " << e.what() << std::endl;
                serial.close();
                run = false;
            }

            if (readLenth > 0) {
                Brytec::CanExtFrame frame;
                uint8_t size = 0;

                Brytec::BinaryDeserializer des;
                des.setData(buffer, 64);

                des.readRaw<uint32_t>(&frame.id);
                des.readRaw<uint8_t>(&frame.dlc);
                des.readRaw<uint8_t>(&frame.data[0]);
                des.readRaw<uint8_t>(&frame.data[1]);
                des.readRaw<uint8_t>(&frame.data[2]);
                des.readRaw<uint8_t>(&frame.data[3]);
                des.readRaw<uint8_t>(&frame.data[4]);
                des.readRaw<uint8_t>(&frame.data[5]);
                des.readRaw<uint8_t>(&frame.data[6]);
                des.readRaw<uint8_t>(&frame.data[7]);

                mutex.lock();
                vec[frame.id] = frame;
                mutex.unlock();
            }
        }
    }

    serial.close();
}

Usb::~Usb()
{
    m_runThread = false;
    if (m_thread.joinable())
        m_thread.join();
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

        if (!m_runThread && m_thread.joinable())
            m_thread.join();

        if (m_serial.isOpen()) {
            m_runThread = true;
            m_thread = std::thread(usbThread, std::ref(m_runThread), std::ref(m_serial), std::ref(m_mutex), std::ref(m_map));
        }
    }
}

void Usb::close()
{
    m_runThread = false;
    m_thread.join();
}

}