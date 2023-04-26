#pragma once

#include "BrytecConfigEmbedded/Can/EBrytecCan.h"
#include "usb/Usb.h"
#include <map>

namespace Brytec {

class SerialWindow {

public:
    SerialWindow() = default;

    void drawWindow();
    void setOpenedState(bool state) { m_opened = state; }
    bool getOpenedState() { return m_opened; }

private:
    bool m_opened = true;
    serial::PortInfo m_selectedDevice;
    Usb m_usb;
    std::vector<CanExtFrame> m_canFrames;
    std::map<uint32_t, CanExtFrame> m_canMap;
};

}