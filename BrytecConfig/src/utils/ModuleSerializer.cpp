#include "ModuleSerializer.h"

#include "AppManager.h"
#include "NodeGroupSerializer.h"
#include "utils/DefaultPaths.h"
#include <filesystem>
#include <fstream>
#include <iostream>

ModuleSerializer::ModuleSerializer(std::shared_ptr<Module>& module)
    : m_module(module)
{
}

ModuleSerializer::ModuleSerializer(std::shared_ptr<Config>& config, std::shared_ptr<Module>& module)
    : m_config(config)
    , m_module(module)
{
}

BinarySerializer ModuleSerializer::serializeTemplateBinary()
{
    BinarySerializer ser;

    // Header
    ser.writeRaw<char>('T');
    ser.writeRaw<char>('M');

    // Version
    ser.writeRaw<uint8_t>(AppManager::getVersion().Major);
    ser.writeRaw<uint8_t>(AppManager::getVersion().Minor);

    // Basic info
    // TODO
    // manufacture name
    // board name
    // version
    // default address

    // Prototype pins
    uint16_t nodeGroupCount = 0;
    ser.writeRaw<uint16_t>(m_module->getPins().size());
    for (auto pin : m_module->getPins()) {
        ser.writeRaw(pin->getPinoutName());
        ser.writeRaw<uint8_t>(pin->getAvailableTypes().size());
        for (auto type : pin->getAvailableTypes())
            ser.writeRaw<uint8_t>((uint8_t)type);
    }

    return ser;
}

BinarySerializer ModuleSerializer::serializeBinary()
{
    BinarySerializer ser;

    // Header
    ser.writeRaw<char>('M');
    ser.writeRaw<char>('D');

    // Version
    ser.writeRaw<uint8_t>(AppManager::getVersion().Major);
    ser.writeRaw<uint8_t>(AppManager::getVersion().Minor);

    // Basic info
    ser.writeRaw<std::string>(m_module->getName());
    ser.writeRaw<uint8_t>(m_module->getAddress());
    ser.writeRaw<uint8_t>(m_module->getEnabled());

    // Count Node Groups
    uint16_t nodeGroupCount = 0;
    for (auto pin : m_module->getPins()) {
        if (auto nodeGroup = pin->getNodeGroup())
            nodeGroupCount++;
    }

    // Node groups
    ser.writeRaw<uint16_t>(nodeGroupCount);
    for (int i = 0; i < m_module->getPins().size(); i++) {
        if (auto nodeGroup = m_module->getPins()[i]->getNodeGroup()) {
            ser.writeRaw<uint16_t>(i);
            NodeGroupSerializer nodeGroupSer(nodeGroup);
            auto nodeGroupBinary = nodeGroupSer.serializeBinary();
            ser.append(nodeGroupBinary);
        }
    }

    return ser;
}

bool ModuleSerializer::deserializeBinary(BinaryDeserializer& des)
{
    // Basic info
    m_module->setName(des.readRaw<std::string>());
    m_module->setAddress(des.readRaw<uint8_t>());
    m_module->setEnabled(des.readRaw<uint8_t>());

    // Prototype pins
    uint32_t pinCount = des.readRaw<uint16_t>();
    for (int i = 0; i < pinCount; i++) {
        std::string pinoutName = des.readRaw<std::string>();

        std::vector<IOTypes::Types> availableTypesVec;
        uint8_t typesCount = des.readRaw<uint8_t>();
        for (int j = 0; j < typesCount; j++)
            availableTypesVec.push_back((IOTypes::Types)des.readRaw<uint8_t>());

        std::shared_ptr<Pin> newPin = std::make_shared<Pin>(pinoutName, availableTypesVec);
        m_module->getPins().push_back(newPin);
    }

    // Node groups
    uint16_t nodeGroupCount = des.readRaw<uint16_t>();
    for (int i = 0; i < nodeGroupCount; i++) {
        uint16_t pinIndex = des.readRaw<uint16_t>();
        std::shared_ptr<NodeGroup> nodeGroup = m_config->addEmptyNodeGroup(0);
        NodeGroupSerializer serializer(nodeGroup);
        if (serializer.deserializeBinary(des)) {
            m_module->getPins()[pinIndex]->setNodeGroup(nodeGroup);
        } else
            return false;
    }

    return true;
}

std::vector<std::filesystem::path> ModuleSerializer::readModulesFromDisk()
{
    std::vector<std::filesystem::path> moduleList;

    // Return empty list if it doesn't exist
    if (!std::filesystem::exists(MODULES_PATH))
        return moduleList;

    for (auto& directoryEntry : std::filesystem::directory_iterator(MODULES_PATH)) {
        const auto& path = directoryEntry.path();

        // Check extension
        if (path.extension() != ".btmodule")
            continue;

        // Ignore directories
        if (directoryEntry.is_directory())
            continue;

        moduleList.push_back(path);
    }

    return moduleList;
}