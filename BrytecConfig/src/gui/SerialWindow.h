#pragma once

#include "BrytecConfigEmbedded/Can/EBrytecCan.h"
#include <map>
#include <mutex>
#include <serial/serial.h>
#include <thread>
#include <vector>

class SerialWindow {

public:
    SerialWindow() = default;
    ~SerialWindow();

    void drawWindow();
    void setOpenedState(bool state) { m_opened = state; }
    bool getOpenedState() { return m_opened; }

private:
    bool m_opened = true;
    std::vector<serial::PortInfo> m_devices;
    serial::PortInfo m_selectedDevice;
    serial::Serial m_serial;
    std::vector<Brytec::CanExtFrame> m_read;
    std::map<uint32_t, Brytec::CanExtFrame> m_map;
    std::mutex m_mutex;
    std::thread m_thread;
    bool m_runThread = false;
};