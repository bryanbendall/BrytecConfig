#include "Config.h"

#include "../utils/ModuleSerializer.h"
#include "Module.h"

Config::Config(const std::filesystem::path& path)
	: m_path(path)
{
}

void Config::addModule(std::filesystem::path modulePath)
{
	std::shared_ptr<Module> module = std::make_shared<Module>();
	ModuleSerializer serializer(module);
	if(serializer.deserializeText(modulePath)) {
		m_modules.push_back(module);
	}
}

void Config::addModule(std::shared_ptr<Module>& module)
{
	m_modules.push_back(module);
}

std::shared_ptr<NodeGroup> Config::addNodeGroup()
{
	std::shared_ptr<NodeGroup> nodeGroup = std::make_shared<NodeGroup>();
	m_nodeGroups.push_back(nodeGroup);
	return nodeGroup;
}

void Config::removeModule(std::shared_ptr<Module>& module)
{
	auto it = std::find(m_modules.begin(), m_modules.end(), module);
	m_modules.erase(it);
}

void Config::removeNodeGroup(std::shared_ptr<NodeGroup>& nodeGroup)
{
	auto it = std::find(m_nodeGroups.begin(), m_nodeGroups.end(), nodeGroup);
	m_nodeGroups.erase(it);
}
