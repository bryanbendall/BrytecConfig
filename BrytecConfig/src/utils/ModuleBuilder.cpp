#include "ModuleBuilder.h"

#include "../AppManager.h"
#include <filesystem>
#include <iostream>
#include <yaml-cpp/yaml.h>
#include <fstream>

ModuleBuilder::ModuleBuilder()
{
	//m_module = std::make_shared<Module>();
}

std::vector<std::filesystem::path> ModuleBuilder::readModulesFromDisk()
{
	std::vector<std::filesystem::path> moduleList;

	// Read file names from disk
	std::filesystem::path modulePath = "data/modules";

	// Return empty list if it doesn't exist
	if(!std::filesystem::exists(modulePath))
		return moduleList;

	for(auto& directoryEntry : std::filesystem::directory_iterator(modulePath)) {
		const auto& path = directoryEntry.path();

		//auto filenameWithExtension = path.filename();
		//auto filenameWithoutExtension = path.stem();
		//auto extension = path.extension();
		
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

void ModuleBuilder::saveModuleFile(std::filesystem::path& filepath, std::shared_ptr<Module> module)
{
	YAML::Emitter out;
	out << YAML::BeginMap;
	out << YAML::Key << "Module" << YAML::Value << module->getName();
	out << YAML::Key << "Pins" << YAML::Value << YAML::BeginSeq;

	for(auto pin : module->getPins()) {
		out << YAML::BeginMap;
		out << YAML::Key << "PinoutName" << YAML::Value << pin->getPinoutName();
		out << YAML::Key << "AvailableTypes" << YAML::Value << YAML::BeginSeq;
		for(auto type : pin->getAvailableTypes())
			out << (unsigned int) type << YAML::Comment(IOTypes::Strings[(int) type]);
		out << YAML::EndSeq;
		out << YAML::EndMap;
	}

	out << YAML::EndSeq;
	out << YAML::EndMap;

	std::ofstream fout(filepath);
	fout << out.c_str();
}

bool ModuleBuilder::readModuleFile(std::filesystem::path& filepath, std::shared_ptr<Module>& outModule)
{
	YAML::Node data = YAML::LoadFile(filepath.string());
	if(!data["Module"])
		return false;

	outModule->getName() = data["Module"].as<std::string>();

	auto pins = data["Pins"];
	if(pins) {

		for(auto pin : pins) {

			std::string pinoutName = pin["PinoutName"].as<std::string>();

			auto availableTypes = pin["AvailableTypes"];
			std::vector<IOTypes::Types> availableTypesVec;
			for(std::size_t i = 0; i < availableTypes.size(); i++) {
				availableTypesVec.push_back((IOTypes::Types)availableTypes[i].as<unsigned int>());
			}

			outModule->getPins().push_back(std::make_shared<Pin>(pinoutName, availableTypesVec));

		}

	}
	

	return true;
}
