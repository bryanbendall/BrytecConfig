#include "Module.h"

#include "../utils/ModuleBuilder.h"

Module::Module() 
{
}

void Module::deletePin(std::shared_ptr<Pin>& pin)
{
	m_pins.erase(std::find(m_pins.begin(), m_pins.end(), pin));
}
