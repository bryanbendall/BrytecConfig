#include "ConfigSerializer.h"

#include <yaml-cpp/yaml.h>
#include <fstream>
#include "ModuleSerializer.h"

ConfigSerializer::ConfigSerializer(std::shared_ptr<Config>& config)
	: m_config(config)
{
}

void ConfigSerializer::serializeText(const std::filesystem::path& filepath)
{
	YAML::Emitter out;
	out << YAML::BeginMap;
	out << YAML::Key << "Config" << YAML::Value << filepath.stem().string();

	{ // Module templates
		out << YAML::Key << "Modules" << YAML::Value << YAML::BeginSeq;
		for(auto module : m_config->getModules()) {
			out << YAML::BeginMap;
			ModuleSerializer moduleSerialzer(module);
			moduleSerialzer.serializeModuleTemplate(out);
			out << YAML::EndMap;
		}
		out << YAML::EndSeq;
		//out << YAML::EndMap;
	}

	{ // Node Groups
		out << YAML::Key << "Node Groups" << YAML::Value << YAML::BeginSeq;
		for(auto nodeGroup : m_config->getNodeGroups()) {
			out << YAML::BeginMap;
			out << YAML::Key << "Name" << YAML::Value << nodeGroup->getName();
			out << YAML::Key << "Type" << YAML::Value << (unsigned int) nodeGroup->getType() << YAML::Comment(IOTypes::getString(nodeGroup->getType()));
			out << YAML::Key << "Enabled" << YAML::Value << nodeGroup->getEnabled();
			out << YAML::EndMap;
		}
		out << YAML::EndSeq;
		//out << YAML::EndMap;
	}

	{ // Node Group assignments

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

	auto modules = data["Modules"];
	if(modules) {
		for(auto module : modules) {
			std::shared_ptr<Module> newModule = std::make_shared<Module>();
			ModuleSerializer moduleSerializer(newModule);
			if(moduleSerializer.deserializeModuleTemplate(module))
				m_config->addModule(newModule);
		}
	}

	auto nodeGroups = data["Node Groups"];
	if(nodeGroups) {
		for(auto nodeGroup : nodeGroups) {
			auto newNodeGroup = m_config->addNodeGroup();
			newNodeGroup->setName(nodeGroup["Name"].as<std::string>());
			newNodeGroup->setType((IOTypes::Types) nodeGroup["Type"].as<unsigned int>());
			newNodeGroup->setEnabled(nodeGroup["Enabled"].as<bool>());
		}
	}

	m_config->setFilepath(filepath);

	return true;
}
