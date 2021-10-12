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
	if(!data["Name"]) {
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

void NodeGroupSerializer::serializeTemplate(YAML::Emitter& out)
{
	out << YAML::Key << "Name" << YAML::Value << m_nodeGroup->getName();
	out << YAML::Key << "Id" << YAML::Value << m_nodeGroup->getId();
	out << YAML::Key << "Type" << YAML::Value << (unsigned int) m_nodeGroup->getType() << YAML::Comment(IOTypes::getString(m_nodeGroup->getType()));
	out << YAML::Key << "Enabled" << YAML::Value << m_nodeGroup->getEnabled();

	// Nodes
	out << YAML::Key << "Nodes" << YAML::Value << YAML::BeginSeq;
	for(auto node : m_nodeGroup->getNodes()) {
		out << YAML::BeginMap;

		out << YAML::Key << "Name" << YAML::Value << node->getName();
		out << YAML::Key << "Type" << YAML::Value << (unsigned int) node->getType() << YAML::Comment(Node::getTypeName(node->getType()));
		// id
		out << YAML::Key << "Position" << YAML::Flow << YAML::BeginSeq << node->getPosition().x << node->getPosition().y << YAML::EndSeq;
		//outputs
		//inputs
		//values

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
