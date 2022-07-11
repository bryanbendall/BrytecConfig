#include "ConfigSerializer.h"

#include "ModuleSerializer.h"
#include "NodeGroupSerializer.h"
#include <fstream>
#include <yaml-cpp/yaml.h>

ConfigSerializer::ConfigSerializer(std::shared_ptr<Config>& config)
    : m_config(config)
{
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

    // Version
    ser.writeRaw<uint8_t>(AppManager::getVersion().Major);
    ser.writeRaw<uint8_t>(AppManager::getVersion().Minor);

    // Config name
    ser.writeRaw(m_config->getName());

    // Module templates
    ser.writeRaw<uint32_t>(m_config->getModules().size());
    for (auto module : m_config->getModules()) {
        ModuleSerializer moduleSer(module);
        auto moduleBinary = moduleSer.serializeTemplateBinary();
        ser.append(moduleBinary);
    }

    // Modules
    ser.writeRaw<uint32_t>(m_config->getModules().size());
    for (auto module : m_config->getModules()) {
        ModuleSerializer moduleSer(module);
        auto moduleBinary = moduleSer.serializeBinary();
        ser.append(moduleBinary);
    }

    // Unassigned node groups count
    uint32_t unassignedNodeGroupCount = 0;
    for (auto ng : m_config->getNodeGroups()) {
        if (!ng->getAssigned())
            unassignedNodeGroupCount++;
    }
    ser.writeRaw<uint32_t>(unassignedNodeGroupCount);

    // Unassigned node groups
    for (auto ng : m_config->getNodeGroups()) {
        if (!ng->getAssigned()) {
            NodeGroupSerializer nodeGroupSer(ng);
            auto nodeGroupBinary = nodeGroupSer.serializeBinary();
            ser.append(nodeGroupBinary);
        }
    }

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