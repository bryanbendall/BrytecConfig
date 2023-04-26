#pragma once

#include "BrytecConfigEmbedded/Can/EBrytecCan.h"
#include "BrytecConfigEmbedded/Usb/UsbDefs.h"
#include <deque>
#include <exception>
#include <map>
#include <mutex>
#include <serial/serial.h>
#include <string>
#include <thread>

namespace Brytec {

class Usb {

public:
    Usb() = default;
    ~Usb();

    void open(std::string port);
    void close();

    bool isOpen() { return m_serial.isOpen(); }
    std::vector<serial::PortInfo> getAvailablePorts() { return serial::list_ports(); }
    std::vector<UsbPacket> getPackets();

    void send(const UsbPacket& packet);

private:
    UsbPacket getPacketFromRaw();

private:
    serial::Serial m_serial;
    bool m_runThread = false;

    std::thread m_rxThread;
    std::mutex m_rxMutex;
    std::vector<uint8_t> m_rxData;

    std::thread m_txThread;
    std::mutex m_txMutex;
    std::deque<UsbPacket> m_txPackets;
};
}