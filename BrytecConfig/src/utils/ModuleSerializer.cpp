#include "ModuleSerializer.h"

#include "../AppManager.h"
#include <filesystem>
#include <iostream>
#include <fstream>

ModuleSerializer::ModuleSerializer(std::shared_ptr<Module>& module)
	: m_module(module)
{
}

ModuleSerializer::ModuleSerializer(std::shared_ptr<Config>& config, std::shared_ptr<Module>& module)
	: m_config(config), m_module(module)
{
}

void ModuleSerializer::serializeTemplateText(const std::filesystem::path& filepath)
{
	YAML::Emitter out;
	out << YAML::BeginMap;

	serializeTemplate(out);

	out << YAML::EndMap;

	std::ofstream fout(filepath);
	fout << out.c_str();
}

bool ModuleSerializer::deserializeTemplateText(const std::filesystem::path& filepath)
{
	YAML::Node data = YAML::LoadFile(filepath.string());
	if(!data["Name"]) {
		assert(false);
		return false;
	}
	return deserializeTemplate(data);
}

void ModuleSerializer::serializeTemplateBinary(const std::filesystem::path& filepath)
{
	assert(false);
}

bool ModuleSerializer::deserializeTemplateBinary(const std::filesystem::path& filepath)
{
	assert(false);
	return false;
}

std::vector<std::filesystem::path> ModuleSerializer::readModulesFromDisk()
{
	std::vector<std::filesystem::path> moduleList;

	// Read file names from disk
	std::filesystem::path modulePath = "data/modules";

	// Return empty list if it doesn't exist
	if(!std::filesystem::exists(modulePath))
		return moduleList;

	for(auto& directoryEntry : std::filesystem::directory_iterator(modulePath)) {
		const auto& path = directoryEntry.path();

		// Check extension
		if(path.extension() != ".yaml")
			continue;

		// Ignore directories
		if(directoryEntry.is_directory())
			continue;

		moduleList.push_back(path);
	}

	return moduleList;
}

void ModuleSerializer::serializeTemplate(YAML::Emitter& out)
{
	out << YAML::Key << "Name" << YAML::Value << m_module->getName();
	out << YAML::Key << "Address" << YAML::Value << (int) m_module->getAddress();
	out << YAML::Key << "Enabled" << YAML::Value << m_module->getEnabled();
	
	out << YAML::Key << "Pins" << YAML::Value << YAML::BeginSeq;
	for(auto pin : m_module->getPins()) {
		out << YAML::BeginMap;
		out << YAML::Key << "Pinout Name" << YAML::Value << pin->getPinoutName();
		out << YAML::Key << "Available Types" << YAML::Value << YAML::BeginSeq;
		for(auto type : pin->getAvailableTypes())
			out << (unsigned int) type << YAML::Comment(IOTypes::getString(type));
		out << YAML::EndSeq;

		if(m_config) {
			if(auto nodeGroup = pin->getNodeGroup())
				out << YAML::Key << "Node Group Id" << YAML::Value << nodeGroup->getId();
		}

		out << YAML::EndMap;
	}
	out << YAML::EndSeq;
}

