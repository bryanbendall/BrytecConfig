#include "Config.h"

#include "../utils/ModuleBuilder.h"
#include "Module.h"

Config::Config(const std::filesystem::path& path)
	: m_path(path)
{
	//for(int i = 0; i < 100; i++)
	//	addNodeGroup();


}

void Config::addModule(std::filesystem::path modulePath)
{
	std::shared_ptr<Module> module = std::make_shared<Module>();
	if(ModuleBuilder::readModuleFile(modulePath, module)) {
		m_modules.push_back(module);
	}
}

void Config::addNodeGroup()
{
	m_nodeGroups.push_back(std::make_shared<NodeGroup>());
}
