#pragma once

#include "BrytecConfigEmbedded/Deserializer/BinaryDeserializer.h"
#include "data/Config.h"
#include "utils/BinarySerializer.h"
#include <filesystem>
#include <memory>

class ConfigSerializer {

public:
    ConfigSerializer(std::shared_ptr<Config>& config);

    BinarySerializer serializeBinary();
    bool deserializeBinary(BinaryDeserializer& des);

private:
    std::shared_ptr<Config> m_config;
};