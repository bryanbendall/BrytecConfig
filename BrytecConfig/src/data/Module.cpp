#include "Module.h"

#include "utils/ModuleSerializer.h"

Module::Module()
{
    addCanBus({ "Can 0", CanTypes::Types::Brytec });
    addCanBus({ "Can 1" });
    addCanBus({ "Can 2" });
    addCanBus({ "Can 3" });
}

void Module::deletePin(std::shared_ptr<Pin>& pin)
{
    m_pins.erase(std::find(m_pins.begin(), m_pins.end(), pin));
}
