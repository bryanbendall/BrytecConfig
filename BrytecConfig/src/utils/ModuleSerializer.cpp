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

void ModuleSerializer::serializeTemplateText(const std::filesystem::path& filepath)
{
    YAML::Emitter out;
    out << YAML::BeginMap;

    serializeTemplate(out);

    out << YAML::EndMap;

    std::ofstream fout(filepath);
    fout << out.c_str();
}

bool ModuleSerializer::deserializeTemplateText(const std::filesystem::path& filepath)
{
    YAML::Node data = YAML::LoadFile(filepath.string());
    if (!data["Name"]) {
        assert(false);
        return false;
    }
    return deserializeTemplate(data);
}

BinarySerializer ModuleSerializer::serializeBinary()
{
    BinarySerializer ser;

    // Basic info
    ser.writeRaw<std::string>(m_module->getName());
    ser.writeRaw<uint8_t>(m_module->getAddress());
    ser.writeRaw<uint8_t>(m_module->getEnabled());

    // Prototype pins
    uint16_t nodeGroupCount = 0;
    ser.writeRaw<uint16_t>(m_module->getPins().size());
    for (auto pin : m_module->getPins()) {
        ser.writeRaw(pin->getPinoutName());
        ser.writeRaw<uint8_t>(pin->getAvailableTypes().size());
        for (auto type : pin->getAvailableTypes())
            ser.writeRaw<uint8_t>((uint8_t)type);

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

void ModuleSerializer::serializeTemplate(YAML::Emitter& out)
{
    out << YAML::Key << "Name" << YAML::Value << m_module->getName();
    out << YAML::Key << "Address" << YAML::Value << (int)m_module->getAddress();
    out << YAML::Key << "Enabled" << YAML::Value << m_module->getEnabled();

    out << YAML::Key << "Pins" << YAML::Value << YAML::BeginSeq;
    for (auto pin : m_module->getPins()) {
        out << YAML::BeginMap;
        out << YAML::Key << "Pinout Name" << YAML::Value << pin->getPinoutName();
        out << YAML::Key << "Available Types" << YAML::Value << YAML::BeginSeq;
        for (auto type : pin->getAvailableTypes())
            out << (unsigned int)type << YAML::Comment(IOTypes::getString(type));
        out << YAML::EndSeq;

        if (m_config) {
            if (auto nodeGroup = pin->getNodeGroup())
                out << YAML::Key << "Node Group Id" << YAML::Value << nodeGroup->getId();
        }

        out << YAML::EndMap;
    }
    out << YAML::EndSeq;
}
