#include "ConfigSerializer.h"

#include "ModuleSerializer.h"
#include "NodeGroupSerializer.h"
#include <fstream>
#include <yaml-cpp/yaml.h>

ConfigSerializer::ConfigSerializer(std::shared_ptr<Config>& config)
    : m_config(config)
{
}

void ConfigSerializer::serializeText(const std::filesystem::path& filepath)
{
    YAML::Emitter out;
    out << YAML::BeginMap;
    out << YAML::Key << "Config" << YAML::Value << filepath.stem().string();

    { // Node Groups
        out << YAML::Key << "Node Groups" << YAML::Value << YAML::BeginSeq;
        for (auto nodeGroup : m_config->getNodeGroups()) {
            out << YAML::BeginMap;
            NodeGroupSerializer nodeGroupSerializer(nodeGroup);
            nodeGroupSerializer.serializeTemplate(out);
            out << YAML::EndMap;
        }
        out << YAML::EndSeq;
    }

    { // Modules
        out << YAML::Key << "Modules" << YAML::Value << YAML::BeginSeq;
        for (auto module : m_config->getModules()) {
            out << YAML::BeginMap;
            ModuleSerializer moduleSerializer(m_config, module);
            moduleSerializer.serializeTemplate(out);
            out << YAML::EndMap;
        }
        out << YAML::EndSeq;
    }

    out << YAML::EndMap;
    std::ofstream fout(filepath);
    fout << out.c_str();
}

bool ConfigSerializer::deserializeText(const std::filesystem::path& filepath)
{
    YAML::Node data = YAML::LoadFile(filepath.string());

    if (!data["Config"]) {
        assert(false);
        return false;
    }

    auto nodeGroups = data["Node Groups"];
    if (nodeGroups) {
        for (auto nodeGroup : nodeGroups) {
            UUID uuid = nodeGroup["Id"].as<uint64_t>();
            auto newNodeGroup = m_config->addEmptyNodeGroup(uuid);
            NodeGroupSerializer nodeGroupSerializer(newNodeGroup);
            assert(nodeGroupSerializer.deserializeTemplate(nodeGroup));
        }
    }

    auto modules = data["Modules"];
    if (modules) {
        for (auto module : modules) {
            std::shared_ptr<Module> newModule = std::make_shared<Module>();
            ModuleSerializer moduleSerializer(m_config, newModule);
            if (moduleSerializer.deserializeTemplate(module))
                m_config->addModule(newModule);
        }
    }

    m_config->setFilepath(filepath);

    return true;
}

BinarySerializer ConfigSerializer::serializeBinary()
{
    BinarySerializer ser;

    // Header
    ser.writeRaw<char>('B');
    ser.writeRaw<char>('r');
    ser.writeRaw<char>('y');
    ser.writeRaw<char>('t');
    ser.writeRaw<char>('e');
    ser.writeRaw<char>('c');

    // Config name
    ser.writeRaw(m_config->getName());

    // Modules
    ser.writeRaw<uint32_t>(m_config->getModules().size());
    for (auto module : m_config->getModules()) {
        ModuleSerializer moduleSer(module);
        auto moduleBinary = moduleSer.serializeBinary();
        ser.append(moduleBinary);
    }

    // Unassigned node groups
    // TODO

    // Footer
    ser.writeRaw<char>('B');
    ser.writeRaw<char>('T');

    return ser;
}

bool ConfigSerializer::deserializeBinary(BinaryDeserializer& des)
{
    // Header
    char header[6];
    header[0] = des.readRaw<char>();
    header[1] = des.readRaw<char>();
    header[2] = des.readRaw<char>();
    header[3] = des.readRaw<char>();
    header[4] = des.readRaw<char>();
    header[5] = des.readRaw<char>();
    if (memcmp(header, "Brytec", 6) != 0)
        return false;

    auto name = des.readRaw<std::string>();

    // Modules
    uint32_t moduleCount = des.readRaw<uint32_t>();
    for (int i = 0; i < moduleCount; i++) {
        std::shared_ptr<Module> module = std::make_shared<Module>();
        ModuleSerializer serializer(m_config, module);
        if (serializer.deserializeBinary(des))
            m_config->addModule(module);
        else
            return false;
    }

    // Test
    return true;

    // Footer
    char footer[2];
    footer[0] = des.readRaw<char>();
    footer[1] = des.readRaw<char>();
    if (memcmp(header, "BT", 2) != 0)
        return false;

    return true;
}