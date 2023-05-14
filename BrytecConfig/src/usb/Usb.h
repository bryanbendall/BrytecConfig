#pragma once

#include "BrytecConfigEmbedded/Can/EBrytecCan.h"
#include "BrytecConfigEmbedded/Usb/UsbDefs.h"
#include "functional"
#include <deque>
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

    void send(const UsbPacket& packet);

    void setReceiveCallback(std::function<void(UsbPacket)> callback) { m_receiveCallback = callback; }

private:
    serial::Serial m_serial;
    bool m_runThread = false;

    std::thread m_rxThread;

    std::thread m_txThread;
    std::mutex m_txMutex;
    std::deque<UsbPacket> m_txPackets;

    std::function<void(UsbPacket)> m_receiveCallback;
};
}