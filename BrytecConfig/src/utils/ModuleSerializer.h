#pragma once

#include "data/Config.h"
#include "data/Module.h"
#include <filesystem>
#include <memory>
#include <vector>
#include <yaml-cpp/yaml.h>

class ModuleSerializer {
public:
    ModuleSerializer(std::shared_ptr<Module>& module);
    ModuleSerializer(std::shared_ptr<Config>& config, std::shared_ptr<Module>& module);

    void serializeTemplateText(const std::filesystem::path& filepath);
    bool deserializeTemplateText(const std::filesystem::path& filepath);

    void serializeTemplateBinary(const std::filesystem::path& filepath);
    bool deserializeTemplateBinary(const std::filesystem::path& filepath);

    BinarySerializer serializeBinary();
    bool deserializeBinary(BinaryDeserializer& des);

private:
    void serializeTemplate(YAML::Emitter& out);

    template <typename T>
    bool deserializeTemplate(T& data)
    {
        m_module->setName(data["Name"].template as<std::string>());

        m_module->setAddress(data["Address"].template as<int>());

        m_module->setEnabled(data["Enabled"].template as<bool>());

        auto pins = data["Pins"];
        if (pins) {
            for (auto pin : pins) {

                std::string pinoutName = pin["Pinout Name"].template as<std::string>();

                auto availableTypes = pin["Available Types"];
                std::vector<IOTypes::Types> availableTypesVec;
                for (std::size_t i = 0; i < availableTypes.size(); i++) {
                    availableTypesVec.push_back((IOTypes::Types)availableTypes[i].template as<unsigned int>());
                }

                std::shared_ptr<Pin> newPin = std::make_shared<Pin>(pinoutName, availableTypesVec);

                if (m_config && pin["Node Group Id"]) {
                    if (auto nodeGroupId = pin["Node Group Id"].template as<uint64_t>()) {
                        newPin->setNodeGroup(m_config->findNodeGroup(nodeGroupId));
                    }
                }

                m_module->getPins().push_back(newPin);
            }
        }

        return true;
    }

public:
    static std::vector<std::filesystem::path> readModulesFromDisk();

private:
    std::shared_ptr<Module> m_module;
    std::shared_ptr<Config> m_config = nullptr;

    friend class ConfigSerializer;
};
