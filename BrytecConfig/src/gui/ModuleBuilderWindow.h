#pragma once

#include "data/Module.h"
#include <memory>

class ModuleBuilderWindow {

public:
    ModuleBuilderWindow();
    void drawWindow();
    void setOpenedState(bool state) { m_opened = state; }

private:
    void drawMenubar();
    void drawModuleTable();
    void drawPinDialog(std::shared_ptr<PhysicalPin>& pin);

private:
    bool m_opened = false;
    std::shared_ptr<Module> m_module;
    std::shared_ptr<PhysicalPin> m_editPin;
};