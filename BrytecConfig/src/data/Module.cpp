#include "Module.h"

#include "InternalPin.h"
#include "PhysicalPin.h"
#include "utils/ModuleSerializer.h"

namespace Brytec {

Module::Module()
{
    addInternalPin();
}

std::shared_ptr<PhysicalPin> Module::addPhysicalPin()
{
    std::shared_ptr<PhysicalPin> pin = std::make_shared<PhysicalPin>("Unnamed", std::vector<IOTypes::Types>(), 0, false);
    m_physicalPins.push_back(pin);
    return pin;
}

void Module::deletePin(std::shared_ptr<PhysicalPin>& pin)
{
    m_physicalPins.erase(std::find(m_physicalPins.begin(), m_physicalPins.end(), pin));
}

void Module::addInternalPin()
{
    std::vector<IOTypes::Types> types = {
        IOTypes::Types::Internal
    };
    m_internalPins.push_back(std::make_shared<InternalPin>(types));
}

void Module::updateInternalPins()
{
    m_internalPins.erase(
        std::remove_if(m_internalPins.begin(), m_internalPins.end(), [](auto& x) {
            return !x->getNodeGroup();
        }),
        m_internalPins.end());

    addInternalPin();
}

CanBus& Module::addCanBus()
{
    CanBus can;

    // Default first can to Brytec
    if (m_canBus.size() == 0)
        can.setType(CanTypes::Types::Brytec);

    m_canBus.push_back(can);

    return m_canBus.back();
}

void Module::deleteCanBus(uint8_t index)
{
    m_canBus.erase(m_canBus.begin() + index);
}
}