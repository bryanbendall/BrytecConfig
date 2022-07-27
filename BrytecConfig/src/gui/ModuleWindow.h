#pragma once

#include "data/Module.h"
#include <imgui.h>
#include <memory>
#include <vector>

class ModuleWindow {

public:
    ModuleWindow();

    void drawWindow();
    void setOpenedState(bool state) { m_opened = state; }
    bool getOpenedState() { return m_opened; }

private:
    void drawMenubar();
    void drawModules();
    void drawModule(std::shared_ptr<Module>& m);

private:
    bool m_opened = true;
};
