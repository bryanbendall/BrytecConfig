#include "ConfigSerializer.h"

#include <yaml-cpp/yaml.h>
#include <fstream>
#include "ModuleSerializer.h"
#include "NodeGroupSerializer.h"

ConfigSerializer::ConfigSerializer(std::shared_ptr<Config>& config)
	: m_config(config)
{
}

void ConfigSerializer::serializeText(const std::filesystem::path& filepath)
{
	YAML::Emitter out;
	out << YAML::BeginMap;
	out << YAML::Key << "Config" << YAML::Value << filepath.stem().string();

	{ // Node Groups
		out << YAML::Key << "Node Groups" << YAML::Value << YAML::BeginSeq;
		for(auto nodeGroup : m_config->getNodeGroups()) {
			out << YAML::BeginMap;
			NodeGroupSerializer nodeGroupSerializer(nodeGroup);
			nodeGroupSerializer.serializeTemplate(out);
			out << YAML::EndMap;
		}
		out << YAML::EndSeq;
	}

	{ // Modules
		out << YAML::Key << "Modules" << YAML::Value << YAML::BeginSeq;
		for(auto module : m_config->getModules()) {
			out << YAML::BeginMap;
			ModuleSerializer moduleSerializer(m_config, module);
			moduleSerializer.serializeTemplate(out);
			out << YAML::EndMap;
		}
		out << YAML::EndSeq;
	}

	out << YAML::EndMap;
	std::ofstream fout(filepath);
	fout << out.c_str();
}

bool ConfigSerializer::deserializeText(const std::filesystem::path& filepath)
{
	YAML::Node data = YAML::LoadFile(filepath.string());
	
	if(!data["Config"]) {
		assert(false);
		return false;
	}

	auto nodeGroups = data["Node Groups"];
	if(nodeGroups) {
		for(auto nodeGroup : nodeGroups) {
			UUID uuid = nodeGroup["Id"].as<uint64_t>();
			auto newNodeGroup = m_config->addEmptyNodeGroup(uuid);
			NodeGroupSerializer nodeGroupSerializer(newNodeGroup);
			assert(nodeGroupSerializer.deserializeTemplate(nodeGroup));
		}
	}

	auto modules = data["Modules"];
	if(modules) {
		for(auto module : modules) {
			std::shared_ptr<Module> newModule = std::make_shared<Module>();
			ModuleSerializer moduleSerializer(m_config, newModule);
			if(moduleSerializer.deserializeTemplate(module))
				m_config->addModule(newModule);
		}
	}

	m_config->setFilepath(filepath);

	return true;
}
