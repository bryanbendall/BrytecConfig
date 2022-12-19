#pragma once

#include "CanBus.h"
#include "InternalPin.h"
#include "PhysicalPin.h"
#include "Pin.h"
#include "Selectable.h"
#include <memory>
#include <string>
#include <vector>

class Module : public Selectable {

public:
    Module();

    std::string& getName() { return m_name; }
    void setName(const std::string& name) { m_name = name; }

    uint8_t& getAddress() { return m_address; }
    void setAddress(const uint8_t& address) { m_address = address; }

    bool& getEnabled() { return m_enabled; }
    void setEnabled(bool state) { m_enabled = state; }

    void addPhysicalPin() { m_physicalPins.push_back(std::make_shared<PhysicalPin>("Unnamed", std::vector<IOTypes::Types>(), 0)); }
    void addPhysicalPin(std::shared_ptr<PhysicalPin>& pin) { m_physicalPins.push_back(pin); }
    void addPhysicalPin(std::string name, std::initializer_list<IOTypes::Types> types, uint8_t maxCurrent) { m_physicalPins.push_back(std::make_shared<PhysicalPin>(name, types, maxCurrent)); }
    void deletePin(std::shared_ptr<PhysicalPin>& pin);
    std::vector<std::shared_ptr<PhysicalPin>>& getPhysicalPins() { return m_physicalPins; }

    void addInternalPin();
    std::vector<std::shared_ptr<InternalPin>>& getInternalPins() { return m_internalPins; }
    void updateInternalPins();

    void addCanBus(CanBus canBus) { m_canBus.push_back(canBus); }
    CanBus& getCanBus(int index) { return m_canBus[index]; }
    std::vector<CanBus>& getCanBuss() { return m_canBus; }

private:
    std::vector<std::shared_ptr<PhysicalPin>> m_physicalPins;
    std::vector<std::shared_ptr<InternalPin>> m_internalPins;
    std::string m_name = "Unnamed";
    uint8_t m_address = 2;
    bool m_enabled = true;
    std::vector<CanBus> m_canBus;
};
