#pragma once

#include "data/Config.h"
#include <filesystem>
#include <memory>

class ConfigSerializer {

public:
    ConfigSerializer(std::shared_ptr<Config>& config);

    void serializeText(const std::filesystem::path& filepath);
    bool deserializeText(const std::filesystem::path& filepath);

    BinarySerializer serializeBinary();
    bool deserializeBinary(BinaryDeserializer& des);

private:
    std::shared_ptr<Config> m_config;
};