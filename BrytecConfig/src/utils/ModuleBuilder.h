#pragma once

#include <memory>
#include "../data/Module.h"
#include <vector>
#include <filesystem>

class ModuleBuilder
{
	ModuleBuilder();

public:
	static std::vector<std::filesystem::path> readModulesFromDisk();
	static void saveModuleFile(std::filesystem::path& filepath, std::shared_ptr<Module> module);
	static bool readModuleFile(std::filesystem::path& filepath, std::shared_ptr<Module>& outModule);

};