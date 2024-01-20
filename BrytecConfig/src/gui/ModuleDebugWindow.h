#pragma once

namespace Brytec {

class ModuleDebugWindow {

public:
    ModuleDebugWindow();
    void drawWindow();
    bool getOpenedState() { return m_opened; }
    void setOpenedState(bool state) { m_opened = state; }

private:
    void drawMenubar();

private:
    bool m_opened = false;
    bool m_openOnMessage = true;
};
}