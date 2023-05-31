#pragma once

#include "data/Module.h"
#include <imgui.h>
#include <memory>
#include <string>
#include <vector>

namespace Brytec {

class Pin;

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
    void drawPinButton(std::shared_ptr<Pin> pin, const std::string& name, float width);

private:
    enum class FilterType {
        All,
        Assigned,
        Unassigned
    };

private:
    bool m_opened = true;
    FilterType m_filter = FilterType::All;
    bool m_simulateModule = false;
};

}