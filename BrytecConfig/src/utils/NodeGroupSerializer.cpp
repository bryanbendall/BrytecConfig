#include "NodeGroupSerializer.h"

#include <fstream>

NodeGroupSerializer::NodeGroupSerializer(std::shared_ptr<NodeGroup>& nodeGroup)
    : m_nodeGroup(nodeGroup)
{
}

void NodeGroupSerializer::serializeTemplateText(const std::filesystem::path& filepath)
{
    YAML::Emitter out;
    out << YAML::BeginMap;

    serializeTemplate(out);

    out << YAML::EndMap;

    std::ofstream fout(filepath);
    fout << out.c_str();
}

bool NodeGroupSerializer::deserializeTemplateText(const std::filesystem::path& filepath)
{
    YAML::Node data = YAML::LoadFile(filepath.string());
    if (!data["Name"]) {
        assert(false);
        return false;
    }
    return deserializeTemplate(data);
}

void NodeGroupSerializer::serializeTemplateBinary(const std::filesystem::path& filepath)
{
    assert(false);
}

bool NodeGroupSerializer::deserializeTemplateBinary(const std::filesystem::path& filepath)
{
    assert(false);
    return false;
}

BinarySerializer NodeGroupSerializer::serializeBinary()
{
    BinarySerializer ser;

    // Basic info
    ser.writeRaw<std::string>(m_nodeGroup->getName());
    ser.writeRaw<uint64_t>(m_nodeGroup->getId());
    ser.writeRaw<uint8_t>((uint8_t)m_nodeGroup->getType());
    ser.writeRaw<uint8_t>(m_nodeGroup->getEnabled());

    // Nodes
    ser.writeRaw<uint16_t>(m_nodeGroup->getNodes().size());
    for (auto node : m_nodeGroup->getNodes()) {
        ser.writeRaw<std::string>(node->getName());
        ser.writeRaw<uint16_t>((uint16_t)node->getType());
        ser.writeRaw<float>(node->getPosition().x);
        ser.writeRaw<float>(node->getPosition().y);

        // Inputs
        ser.writeRaw<uint8_t>(node->getInputs().size());
        for (auto input : node->getInputs()) {
            if (!input.ConnectedNode.expired()) {
                ser.writeRaw<int8_t>(m_nodeGroup->getNodeIndex(input.ConnectedNode.lock()));
                ser.writeRaw<int8_t>(input.OutputIndex);
                ser.writeRaw<float>(input.DefaultValue);
            } else {
                ser.writeRaw<int8_t>(-1);
                ser.writeRaw<int8_t>(-1);
                ser.writeRaw<float>(input.DefaultValue);
            }
        }

        // Values
        ser.writeRaw<uint8_t>(node->getValues().size());
        for (float value : node->getValues())
            ser.writeRaw<float>(value);

        // Node Group reference (special case for NodeGroup Node)
        // TODO
    }

    return ser;
}

void NodeGroupSerializer::serializeTemplate(YAML::Emitter& out)
{
    out << YAML::Key << "Name" << YAML::Value << m_nodeGroup->getName();
    out << YAML::Key << "Id" << YAML::Value << m_nodeGroup->getId();
    out << YAML::Key << "Type" << YAML::Value << (unsigned int)m_nodeGroup->getType() << YAML::Comment(IOTypes::getString(m_nodeGroup->getType()));
    out << YAML::Key << "Enabled" << YAML::Value << m_nodeGroup->getEnabled();

    // Nodes
    out << YAML::Key << "Nodes" << YAML::Value << YAML::BeginSeq;
    for (auto node : m_nodeGroup->getNodes()) {
        out << YAML::BeginMap;

        if (!node->getName().empty())
            out << YAML::Key << "Name" << YAML::Value << node->getName();
        out << YAML::Key << "Type" << YAML::Value << (unsigned int)node->getType() << YAML::Comment(Node::s_getTypeName(node->getType()));
        out << YAML::Key << "Position" << YAML::Flow << YAML::BeginSeq << node->getPosition().x << node->getPosition().y << YAML::EndSeq;

        // Outputs - dont need to serialize because they are calculated

        // Inputs
        if (node->getInputs().size() > 0) {
            out << YAML::Key << "Inputs" << YAML::BeginSeq;
            for (auto input : node->getInputs()) {
                out << YAML::Flow << YAML::BeginSeq;
                if (!input.ConnectedNode.expired()) {
                    out << m_nodeGroup->getNodeIndex(input.ConnectedNode.lock()) << input.OutputIndex << input.DefaultValue;
                } else {
                    out << -1 << -1 << input.DefaultValue;
                }
                out << YAML::EndSeq;
            }
            out << YAML::EndSeq;
        }

        // Values
        if (node->getValues().size() > 0) {
            out << YAML::Key << "Values" << YAML::Flow << YAML::BeginSeq;
            for (float value : node->getValues())
                out << value;
            out << YAML::EndSeq;
        }

        // Node Group reference (special case for NodeGroup Node)
        std::shared_ptr<NodeGroup> selectedNodeGroup = AppManager::getConfig()->findNodeGroup(node->getSelectedNodeGroup());
        if (selectedNodeGroup)
            out << YAML::Key << "Node Group ID" << selectedNodeGroup->getId();

        out << YAML::EndMap;
    }
    out << YAML::EndSeq;
#if 0
	out << YAML::Key << "Pins" << YAML::Value << YAML::BeginSeq;
	for(auto pin : m_module->getPins()) {
		out << YAML::BeginMap;
		out << YAML::Key << "PinoutName" << YAML::Value << pin->getPinoutName();
		out << YAML::Key << "AvailableTypes" << YAML::Value << YAML::BeginSeq;
		for(auto type : pin->getAvailableTypes())
			out << (unsigned int) type << YAML::Comment(IOTypes::getString(type));
		out << YAML::EndSeq;
		out << YAML::EndMap;
	}
	out << YAML::EndSeq;
#endif
}
