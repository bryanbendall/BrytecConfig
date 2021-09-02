#pragma once

#include <vector>
#include "Module.h"
#include <memory>
#include "NodeGroup.h"
#include <filesystem>

class Config
{
	std::vector<std::shared_ptr<Module>> m_modules;
	std::vector<std::shared_ptr<NodeGroup>> m_nodeGroups;

public:
	Config();
	const std::vector<std::shared_ptr<Module>>& getModules() { return m_modules; }
	const std::vector<std::shared_ptr<NodeGroup>>& getNodeGroups() { return m_nodeGroups; }
	void addModule(std::filesystem::path modulePath);
	void addNodeGroup();

};

