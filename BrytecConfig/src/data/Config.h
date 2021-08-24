#pragma once

#include <vector>
#include "Module.h"
#include <memory>

class Config
{

	std::vector<std::shared_ptr<Module>> m_modules;

public:
	Config();
	const std::vector<std::shared_ptr<Module>>& getModules() { return m_modules; }
	void addModule(ModuleTypes type);

};

