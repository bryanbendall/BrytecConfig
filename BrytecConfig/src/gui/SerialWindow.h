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
};

}