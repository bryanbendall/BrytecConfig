#pragma once

namespace Brytec {

class SettingsWindow {

public:
    SettingsWindow();
    void drawWindow();
    void setOpenedState(bool state) { m_opened = state; }

private:
    bool m_opened = false;
};

}