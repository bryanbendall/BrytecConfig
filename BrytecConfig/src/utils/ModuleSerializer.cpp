#include "ModuleSerializer.h"

#include "AppManager.h"
#include "NodeGroupSerializer.h"
#include <filesystem>
#include <fstream>
#include <iostream>

namespace Brytec {

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
    ser.writeRaw<std::string>(m_module->getName());
    ser.writeRaw<std::string>(m_module->getManufacturerName());
    ser.writeRaw<std::string>(m_module->getBoardName());
    ser.writeRaw<uint8_t>(m_module->getAddress());

    // Prototype pins
    ser.writeRaw<uint16_t>(m_module->getPhysicalPins().size());
    for (auto pin : m_module->getPhysicalPins()) {
        // Name
        ser.writeRaw(pin->getPinoutName());
        // Types
        ser.writeRaw<uint8_t>(pin->getAvailableTypes().size());
        for (auto type : pin->getAvailableTypes())
            ser.writeRaw<uint8_t>((uint8_t)type);
        // Max Current
        ser.writeRaw<uint8_t>(pin->getMaxCurrent());
        // Pwm
        ser.writeRaw<uint8_t>(pin->getPwm());
    }

    return ser;
}

bool ModuleSerializer::deserializeTemplateBinary(BinaryDeserializer& des)
{
    char t, m;
    des.readRaw<char>(&t); // T
    des.readRaw<char>(&m); // M
    // TODO check header

    uint8_t major, minor;
    des.readRaw<uint8_t>(&major);
    des.readRaw<uint8_t>(&minor);
    // TODO check version

    // Basic Info
    std::string name;
    des.readRaw<std::string>(&name);
    m_module->setName(name);

    std::string manufacturer;
    des.readRaw<std::string>(&manufacturer);
    m_module->setManufacturerName(manufacturer);

    std::string board;
    des.readRaw<std::string>(&board);
    m_module->setBoardName(board);

    uint8_t address;
    des.readRaw<uint8_t>(&address);
    m_module->setAddress(address);

    // Pins
    uint16_t pinCount;
    des.readRaw<uint16_t>(&pinCount);
    for (int i = 0; i < pinCount; i++) {
        // Name
        std::string pinoutName;
        des.readRaw<std::string>(&pinoutName);

        // Types
        std::vector<IOTypes::Types> availableTypesVec;
        uint8_t typesCount;
        des.readRaw<uint8_t>(&typesCount);
        for (int j = 0; j < typesCount; j++) {
            uint8_t type;
            des.readRaw<uint8_t>(&type);
            availableTypesVec.push_back((IOTypes::Types)type);
        }

        // Max Current
        uint8_t maxCurrent;
        des.readRaw<uint8_t>(&maxCurrent);

        // Pwm
        uint8_t pwm;
        des.readRaw<uint8_t>(&pwm);

        // Make pin
        std::shared_ptr<PhysicalPin> newPin = std::make_shared<PhysicalPin>(pinoutName, availableTypesVec, maxCurrent, pwm);
        m_module->getPhysicalPins().push_back(newPin);
    }

    return true;
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
    ser.writeRaw<std::string>(m_module->getManufacturerName());
    ser.writeRaw<std::string>(m_module->getBoardName());
    ser.writeRaw<uint8_t>(m_module->getAddress());
    ser.writeRaw<uint8_t>(m_module->getEnabled());

    // Node Group Count
    uint16_t physicalNodeGroupCount = 0;
    for (auto pin : m_module->getPhysicalPins()) {
        if (auto nodeGroup = pin->getNodeGroup())
            physicalNodeGroupCount++;
    }

    uint16_t internalNodeGroupCount = 0;
    for (auto pin : m_module->getInternalPins()) {
        if (auto nodeGroup = pin->getNodeGroup())
            internalNodeGroupCount++;
    }

    ser.writeRaw<uint16_t>(physicalNodeGroupCount + internalNodeGroupCount);

    // Physical Pin Node Groups
    {
        ser.writeRaw<uint16_t>(physicalNodeGroupCount);
        for (int i = 0; i < m_module->getPhysicalPins().size(); i++) {
            if (auto nodeGroup = m_module->getPhysicalPins()[i]->getNodeGroup()) {
                ser.writeRaw<uint16_t>(i); // pin index
                NodeGroupSerializer nodeGroupSer(nodeGroup);
                auto nodeGroupBinary = nodeGroupSer.serializeBinary();
                ser.append(nodeGroupBinary);
            }
        }
    }

    // Internal Pin Node Groups
    {
        ser.writeRaw<uint16_t>(internalNodeGroupCount);
        for (uint16_t i = 0; i < m_module->getInternalPins().size(); i++) {
            if (auto nodeGroup = m_module->getInternalPins()[i]->getNodeGroup()) {
                ser.writeRaw<uint16_t>(i + m_module->getPhysicalPins().size()); // pin index
                NodeGroupSerializer nodeGroupSer(nodeGroup);
                auto nodeGroupBinary = nodeGroupSer.serializeBinary();
                ser.append(nodeGroupBinary);
            }
        }
    }

    return ser;
}

bool ModuleSerializer::deserializeBinary(BinaryDeserializer& des)
{
    char m, d;
    des.readRaw<char>(&m); // M
    des.readRaw<char>(&d); // D
    // TODO check header

    uint8_t major, minor;
    des.readRaw<uint8_t>(&major);
    des.readRaw<uint8_t>(&minor);
    // TODO check version

    // Basic info
    std::string name;
    des.readRaw<std::string>(&name);
    m_module->setName(name);

    std::string manufacturer;
    des.readRaw<std::string>(&manufacturer);
    m_module->setManufacturerName(manufacturer);

    std::string board;
    des.readRaw<std::string>(&board);
    m_module->setBoardName(board);

    uint8_t address;
    des.readRaw<uint8_t>(&address);
    m_module->setAddress(address);

    uint8_t enabled;
    des.readRaw<uint8_t>(&enabled);
    m_module->setEnabled(enabled);

    uint16_t totalNodeGroups;
    des.readRaw<uint16_t>(&totalNodeGroups);

    // Physical Pin Node Groups
    {
        uint16_t nodeGroupCount;
        des.readRaw<uint16_t>(&nodeGroupCount);
        for (int i = 0; i < nodeGroupCount; i++) {
            uint16_t pinIndex;
            des.readRaw<uint16_t>(&pinIndex);
            std::shared_ptr<NodeGroup> nodeGroup = m_config->addEmptyNodeGroup(0);
            NodeGroupSerializer serializer(nodeGroup);
            if (serializer.deserializeBinary(des)) {
                m_module->getPhysicalPins()[pinIndex]->setNodeGroup(nodeGroup);
            } else
                return false;
        }
    }

    // Internal Pin Node Groups
    {
        uint16_t nodeGroupCount;
        des.readRaw<uint16_t>(&nodeGroupCount);
        for (int i = 0; i < nodeGroupCount; i++) {
            uint16_t pinIndex;
            des.readRaw<uint16_t>(&pinIndex);
            std::shared_ptr<NodeGroup> nodeGroup = m_config->addEmptyNodeGroup(0);
            NodeGroupSerializer serializer(nodeGroup);
            if (serializer.deserializeBinary(des)) {
                m_module->addInternalPin();
                m_module->getInternalPins()[pinIndex - m_module->getPhysicalPins().size()]->setNodeGroup(nodeGroup);
            } else
                return false;
        }
    }

    return true;
}

std::vector<std::filesystem::path> ModuleSerializer::readModulesFromDisk()
{
    std::vector<std::filesystem::path> moduleList;

    // Return empty list if it doesn't exist
    if (!std::filesystem::exists(AppManager::getModulesPath()))
        return moduleList;

    for (auto& directoryEntry : std::filesystem::directory_iterator(AppManager::getModulesPath())) {
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

}