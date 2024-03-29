#pragma once

#include "data/CanBus.h"
#include <memory>

namespace Brytec {

class Module;
class Pin;
class NodeGroup;

class PropertiesWindow {

public:
    PropertiesWindow() = default;

    void drawWindow();
    void setOpenedState(bool state) { m_opened = state; }
    bool getOpenedState() { return m_opened; }

private:
    void drawModuleProps(std::shared_ptr<Module> module);
    void drawCanBus(CanBus& can, uint8_t index);
    void drawPinProps(std::shared_ptr<Pin> pin);
    void drawNodeGroupProps(std::shared_ptr<NodeGroup> nodeGroup);

private:
    bool m_opened = true;
};

}