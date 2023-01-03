#include "Module.h"

#include "utils/ModuleSerializer.h"

Module::Module()
{
    addCanBus({ "Can 0", CanTypes::Types::Brytec });
    addCanBus({ "Can 1" });
    addCanBus({ "Can 2" });
    addCanBus({ "Can 3" });

    addInternalPin();
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
