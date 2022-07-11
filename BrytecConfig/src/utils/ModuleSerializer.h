#pragma once

#include "BrytecConfigEmbedded/Utils/BinaryDeserializer.h"
#include "data/Config.h"
#include "data/Module.h"
#include "utils/BinarySerializer.h"
#include <filesystem>
#include <memory>
#include <vector>
#include <yaml-cpp/yaml.h>

class ModuleSerializer {
public:
    ModuleSerializer(std::shared_ptr<Module>& module);
    ModuleSerializer(std::shared_ptr<Config>& config, std::shared_ptr<Module>& module);

    BinarySerializer serializeTemplateBinary();
    BinarySerializer serializeBinary();
    bool deserializeBinary(BinaryDeserializer& des);

public:
    static std::vector<std::filesystem::path> readModulesFromDisk();

private:
    std::shared_ptr<Module> m_module;
    std::shared_ptr<Config> m_config = nullptr;

    friend class ConfigSerializer;
};
