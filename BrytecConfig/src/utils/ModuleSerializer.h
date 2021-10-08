#pragma once

#include <memory>
#include "../data/Module.h"
#include <vector>
#include <filesystem>
#include <yaml-cpp/yaml.h>

class ModuleSerializer
{
public:
	ModuleSerializer(std::shared_ptr<Module>& module);

	void serializeText(const std::filesystem::path& filepath);
	bool deserializeText(const std::filesystem::path& filepath);

private:
	void serializeModuleTemplate(YAML::Emitter& out);
	template<typename T>
	bool deserializeModuleTemplate(T& data)
	{
		m_module->setName(data["Name"].as<std::string>());

		m_module->setAddress(data["Address"].as<int>());

		m_module->setEnabled(data["Enabled"].as<bool>());

		auto pins = data["Pins"];
		if(pins) {
			for(auto pin : pins) {

				std::string pinoutName = pin["PinoutName"].as<std::string>();

				auto availableTypes = pin["AvailableTypes"];
				std::vector<IOTypes::Types> availableTypesVec;
				for(std::size_t i = 0; i < availableTypes.size(); i++) {
					availableTypesVec.push_back((IOTypes::Types) availableTypes[i].as<unsigned int>());
				}

				m_module->getPins().push_back(std::make_shared<Pin>(pinoutName, availableTypesVec));

			}
		}

		return true;
	}

public:
	static std::vector<std::filesystem::path> readModulesFromDisk();

private:
	std::shared_ptr<Module>& m_module;

	friend class ConfigSerializer;
};


