#pragma once

#include "BrytecConfigEmbedded/Can/EBrytecCan.h"
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
    std::map<uint32_t, CanExtFrame> getMap() { return m_map; }

private:
    serial::Serial m_serial;
    bool m_runThread = false;
    std::thread m_thread;
    std::mutex m_mutex;
    std::map<uint32_t, CanExtFrame> m_map;
};
}