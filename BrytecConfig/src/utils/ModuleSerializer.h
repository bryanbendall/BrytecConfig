#pragma once

#include "BrytecConfigEmbedded/Deserializer/BinaryPathDeserializer.h"
#include "data/Config.h"
#include "data/Module.h"
#include "utils/BinarySerializer.h"
#include <filesystem>
#include <memory>
#include <vector>
#include <yaml-cpp/yaml.h>

namespace Brytec {

class ModuleSerializer {
public:
    ModuleSerializer(std::shared_ptr<Module>& module);
    ModuleSerializer(std::shared_ptr<Config>& config, std::shared_ptr<Module>& module);

    BinarySerializer serializeTemplateBinary();
    bool deserializeTemplateBinary(BinaryDeserializer& des);
    BinarySerializer serializeBinary();
    bool deserializeBinary(BinaryDeserializer& des);

public:
    static std::vector<std::filesystem::path> readModulesFromDisk();

private:
    std::shared_ptr<Module> m_module;
    std::shared_ptr<Config> m_config = nullptr;
};

}
