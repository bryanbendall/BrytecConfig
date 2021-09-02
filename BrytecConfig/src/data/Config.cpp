#include "Config.h"

#include "../utils/ModuleBuilder.h"
#include "Module.h"

Config::Config()
{
	for(int i = 0; i < 100; i++)
		addNodeGroup();

	std::filesystem::path filepath = "data/modules/newMod.yaml";
	std::shared_ptr<Module> module = std::make_shared<Module>();
	auto& pins = module->getPins();
	std::vector<IOTypes::Types> pinTypes;
	pinTypes.push_back(IOTypes::Types::Input_12V);
	pinTypes.push_back(IOTypes::Types::Input_Can);
	pinTypes.push_back(IOTypes::Types::Output_Ground);
	pins.push_back(std::make_shared<Pin>("Pin1", pinTypes));
	pins.push_back(std::make_shared<Pin>("B9", pinTypes));
	pins.push_back(std::make_shared<Pin>("sfdsfsd", pinTypes));

	ModuleBuilder::saveModuleFile(filepath, module);
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
