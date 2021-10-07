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

	{ // Module templates
		out << YAML::BeginMap;
		out << YAML::Key << "Config" << YAML::Value << filepath.stem().string();
		out << YAML::Key << "Modules" << YAML::Value << YAML::BeginSeq;
		for(auto module : m_config->getModules()) {
			out << YAML::BeginMap;
			ModuleSerializer moduleSerialzer(module);
			moduleSerialzer.serializeModuleTemplate(out);
			out << YAML::EndMap;
		}
		out << YAML::EndSeq;
		out << YAML::EndMap;
	}

	{ // Node Groups

	}

	{ // Node Group assignments

	}

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

	m_config->setFilepath(filepath);

	return true;
}
