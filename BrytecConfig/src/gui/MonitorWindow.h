#pragma once

namespace Brytec {

class MonitorWindow {

public:
    MonitorWindow();
    void drawWindow();
    bool getOpenedState() { return m_opened; }
    void setOpenedState(bool state) { m_opened = state; }

private:
    bool m_opened = true;
};
}