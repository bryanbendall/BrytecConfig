#pragma once

#include "data/Module.h"
#include <memory>

class ModuleBuilderWindow {
    bool m_opened = false;
    std::shared_ptr<Module> m_module;

public:
    ModuleBuilderWindow();
    void drawWindow();
    void setOpenedState(bool state) { m_opened = state; }

private:
    void drawMenubar();
    void drawModuleTable();
};