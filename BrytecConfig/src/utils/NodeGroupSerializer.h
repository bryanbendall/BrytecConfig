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

		// Add Nodes
		if(auto nodes = data["Nodes"]) {
			for(auto node : nodes) {
				node["Name"].as<std::string>();
				NodeTypes type = (NodeTypes)node["Type"].as<unsigned int>();
				float x = node["Position"][0].as<float>();
				float y = node["Position"][1].as<float>();

				auto newNode = m_nodeGroup->addNode(type, {x, y});
			}
		}

		// Add connection to nodes previously added
		if(auto nodes = data["Nodes"]) {
			int nodeIndex = 0;
			for(auto node : nodes) {
				if(auto inputs = node["Inputs"]) {
					int inputIndex = 0;
					for(auto input : inputs) {
						auto connectionNodeIndex = input[0].as<int>();
						auto outputIndex = input[1].as<int>();
						if(connectionNodeIndex > -1 && outputIndex > -1) {
							// Has connection
							NodeConnection nodeConnection = {m_nodeGroup->getNodes()[connectionNodeIndex], outputIndex};
							m_nodeGroup->getNodes()[nodeIndex]->setInput(inputIndex, nodeConnection);
						}
						inputIndex++;
					}
				}
				nodeIndex++;
			}
		}

		return true;
	}

private:
	std::shared_ptr<NodeGroup> m_nodeGroup;

	friend class ConfigSerializer;

};