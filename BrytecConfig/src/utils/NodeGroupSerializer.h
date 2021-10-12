#pragma once

#include "../data/NodeGroup.h"
#include <memory>
#include <filesystem>
#include <yaml-cpp/yaml.h>

class NodeGroupSerializer
{

public:
	NodeGroupSerializer(std::shared_ptr<NodeGroup>& nodeGroup);

	void serializeTemplateText(const std::filesystem::path& filepath);
	bool deserializeTemplateText(const std::filesystem::path& filepath);

	void serializeTemplateBinary(const std::filesystem::path& filepath);
	bool deserializeTemplateBinary(const std::filesystem::path& filepath);

private:
	void serializeTemplate(YAML::Emitter& out);

	template<typename T>
	bool deserializeTemplate(T& data)
	{
		m_nodeGroup->setName(data["Name"].as<std::string>());

		m_nodeGroup->setType((IOTypes::Types) data["Type"].as<unsigned int>());

		m_nodeGroup->setEnabled(data["Enabled"].as<bool>());

		return true;
#if 0
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
#endif
	}

private:
	std::shared_ptr<NodeGroup> m_nodeGroup;

	friend class ConfigSerializer;

};