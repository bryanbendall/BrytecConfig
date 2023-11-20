#pragma once

#include "CanBus.h"
#include "Selectable.h"
#include <memory>
#include <string>
#include <vector>

namespace Brytec {

class PhysicalPin;
class InternalPin;

class Module : public Selectable {

public:
    Module();

    std::string& getName() { return m_name; }
    void setName(const std::string& name) { m_name = name; }

    std::string& getManufacturerName() { return m_manufacturer; }
    void setManufacturerName(const std::string& name) { m_manufacturer = name; }

    std::string& getBoardName() { return m_boardName; }
    void setBoardName(const std::string& name) { m_boardName = name; }

    uint8_t& getAddress() { return m_address; }
    void setAddress(const uint8_t& address) { m_address = address; }

    bool& getEnabled() { return m_enabled; }
    void setEnabled(bool state) { m_enabled = state; }

    std::shared_ptr<PhysicalPin> addPhysicalPin();
    void deletePin(std::shared_ptr<PhysicalPin>& pin);
    std::vector<std::shared_ptr<PhysicalPin>>& getPhysicalPins() { return m_physicalPins; }

    void addInternalPin();
    std::vector<std::shared_ptr<InternalPin>>& getInternalPins() { return m_internalPins; }
    void updateInternalPins();

    CanBus& addCanBus();
    CanBus& getCanBus(int index) { return m_canBus[index]; }
    std::vector<CanBus>& getCanBuses() { return m_canBus; }
    void deleteCanBus(uint8_t index);

private:
    std::vector<std::shared_ptr<PhysicalPin>> m_physicalPins;
    std::vector<std::shared_ptr<InternalPin>> m_internalPins;
    std::string m_name = "Unnamed";
    std::string m_manufacturer = "None";
    std::string m_boardName = "None";
    uint8_t m_address = 2;
    bool m_enabled = true;
    std::vector<CanBus> m_canBus;
};

}