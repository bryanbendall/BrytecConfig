#pragma once

#include "AppManager.h"
#include "BrytecConfigEmbedded/Nodes.h"
#include "data/NodeGroup.h"
#include <filesystem>
#include <memory>
#include <yaml-cpp/yaml.h>

class NodeGroupSerializer {

public:
    NodeGroupSerializer(std::shared_ptr<NodeGroup>& nodeGroup);

    void serializeTemplateText(const std::filesystem::path& filepath);
    bool deserializeTemplateText(const std::filesystem::path& filepath);

    void serializeTemplateBinary(const std::filesystem::path& filepath);
    bool deserializeTemplateBinary(const std::filesystem::path& filepath);

private:
    void serializeTemplate(YAML::Emitter& out);

    template <typename T>
    bool deserializeTemplate(T& data)
    {
        m_nodeGroup->setName(data["Name"].template as<std::string>());

        m_nodeGroup->setType((IOTypes::Types)data["Type"].template as<unsigned int>());

        m_nodeGroup->setEnabled(data["Enabled"].template as<bool>());

        // Add Nodes
        if (auto nodes = data["Nodes"]) {
            for (auto node : nodes) {
                Embedded::NodeTypes type = (Embedded::NodeTypes)node["Type"].template as<unsigned int>();
                float x = node["Position"][0].template as<float>();
                float y = node["Position"][1].template as<float>();

                auto newNode = m_nodeGroup->addNode(type, { x, y });

                if (auto nodeName = node["Name"]) {
                    std::string name = node["Name"].template as<std::string>();
                    newNode->setName(name);
                }

                if (auto values = node["Values"]) {
                    int valueIndex = 0;
                    for (auto value : values) {
                        newNode->setValue(valueIndex, value.template as<float>());
                        valueIndex++;
                    }
                }
            }
        }

        // Add connection to nodes previously added
        if (auto nodes = data["Nodes"]) {
            int nodeIndex = 0;
            for (auto node : nodes) {
                if (auto inputs = node["Inputs"]) {
                    int inputIndex = 0;
                    for (auto input : inputs) {
                        auto connectionNodeIndex = input[0].template as<int>();
                        auto outputIndex = input[1].template as<int>();
                        auto defaultValue = input[2].template as<float>();
                        if (connectionNodeIndex > -1 && outputIndex > -1) {
                            // Has connection
                            NodeConnection nodeConnection = { m_nodeGroup->getNodes()[connectionNodeIndex], outputIndex, defaultValue };
                            m_nodeGroup->getNodes()[nodeIndex]->setInput(inputIndex, nodeConnection);
                        } else {
                            m_nodeGroup->getNodes()[nodeIndex]->getInput(inputIndex).DefaultValue = defaultValue;
                        }
                        inputIndex++;
                    }
                }
                if (auto nodeGroupId = node["Node Group ID"]) {
                    m_nodeGroup->getNodes()[nodeIndex]->setSelectedNodeGroup(nodeGroupId.template as<uint64_t>());
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