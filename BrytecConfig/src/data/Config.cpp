#include "Config.h"

Config::Config()
{
	m_modules.push_back(std::make_unique<Module>(ModuleTypes::Input));
	m_modules.push_back(std::make_unique<Module>(ModuleTypes::Power));
	m_modules.push_back(std::make_unique<Module>(ModuleTypes::Power));
	m_modules.push_back(std::make_unique<Module>(ModuleTypes::Power));
	m_modules.push_back(std::make_unique<Module>(ModuleTypes::Power));
}

void Config::addModule(ModuleTypes type) 
{
	m_modules.push_back(std::make_unique<Module>(type));
}
