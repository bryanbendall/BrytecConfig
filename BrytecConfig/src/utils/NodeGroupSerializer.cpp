#include "NodeGroupSerializer.h"

#include <fstream>

NodeGroupSerializer::NodeGroupSerializer(std::shared_ptr<NodeGroup>& nodeGroup)
    : m_nodeGroup(nodeGroup)
{
}

BinarySerializer NodeGroupSerializer::serializeBinary()
{
    BinarySerializer ser;

    // Header
    ser.writeRaw<char>('N');
    ser.writeRaw<char>('G');

    // Version
    ser.writeRaw<uint8_t>(AppManager::getVersion().Major);
    ser.writeRaw<uint8_t>(AppManager::getVersion().Minor);

    // Basic info
    ser.writeRaw<std::string>(m_nodeGroup->getName());
    ser.writeRaw<uint64_t>(m_nodeGroup->getId());
    ser.writeRaw<uint8_t>((uint8_t)m_nodeGroup->getType());
    ser.writeRaw<uint8_t>(m_nodeGroup->getEnabled());

    // Current limit
    ser.writeRaw<uint8_t>(m_nodeGroup->getCurrentLimit());
    ser.writeRaw<uint8_t>(m_nodeGroup->getAlwaysRetry());
    ser.writeRaw<uint8_t>(m_nodeGroup->getMaxRetries());
    ser.writeRaw<float>(m_nodeGroup->getRetryDelay());

    // Nodes
    ser.writeRaw<uint16_t>(m_nodeGroup->getNodes().size());
    for (auto node : m_nodeGroup->getNodes()) {
        ser.writeRaw<std::string>(node->getName());
        ser.writeRaw<uint16_t>((uint16_t)node->getType());
        ser.writeRaw<float>(node->getPosition().x);
        ser.writeRaw<float>(node->getPosition().y);

        // Inputs
        ser.writeRaw<uint8_t>(node->getInputs().size());
        for (auto input : node->getInputs()) {
            if (!input.ConnectedNode.expired())
                ser.writeRaw<int8_t>(ConnectionType::Pointer);
            else
                ser.writeRaw<int8_t>(ConnectionType::Float);
        }

        // Values
        ser.writeRaw<uint8_t>(node->getValues().size());
        for (float value : node->getValues())
            ser.writeRaw<float>(value);

        // Node Group reference (special case for NodeGroup Node)
        if (node->getType() == NodeTypes::Node_Group) {
            ser.writeRaw<uint64_t>(node->getSelectedNodeGroup());
            std::shared_ptr<NodeGroup> selectedNodeGroup = AppManager::getConfig()->findNodeGroup(node->getSelectedNodeGroup());
            if (selectedNodeGroup) {
                ser.writeRaw<uint8_t>(AppManager::getConfig()->getAssignedModuleAddress(selectedNodeGroup)); // Module address
                ser.writeRaw<uint8_t>(AppManager::getConfig()->getAssignedPinAddress(selectedNodeGroup)); // Pin index
            } else {
                ser.writeRaw<uint8_t>(0);
                ser.writeRaw<uint8_t>(0);
            }
        }
    }

    // Node Connections
    ser.writeRaw<uint16_t>(m_nodeGroup->getNodes().size());
    for (auto node : m_nodeGroup->getNodes()) {

        // Inputs
        ser.writeRaw<uint8_t>(node->getInputs().size());
        for (auto input : node->getInputs()) {
            if (!input.ConnectedNode.expired()) {
                ser.writeRaw<int8_t>(m_nodeGroup->getNodeIndex(input.ConnectedNode.lock()));
                ser.writeRaw<int8_t>(input.OutputIndex);
                ser.writeRaw<float>(input.DefaultValue);
            } else {
                ser.writeRaw<int8_t>(-1);
                ser.writeRaw<int8_t>(-1);
                ser.writeRaw<float>(input.DefaultValue);
            }
        }
    }

    return ser;
}

bool NodeGroupSerializer::deserializeBinary(BinaryDeserializer& des)
{
    // Header
    char n, g;
    des.readRaw<char>(&n); // N
    des.readRaw<char>(&g); // G

    // Version
    uint8_t major, minor;
    des.readRaw<uint8_t>(&major);
    des.readRaw<uint8_t>(&minor);

    // Basic info
    std::string name;
    des.readRaw<std::string>(&name);
    m_nodeGroup->setName(name);

    uint64_t uuid;
    des.readRaw<uint64_t>(&uuid);
    m_nodeGroup->setId(uuid);

    uint8_t type;
    des.readRaw<uint8_t>(&type);
    m_nodeGroup->setType((IOTypes::Types)type);

    uint8_t enabled;
    des.readRaw<uint8_t>(&enabled);
    m_nodeGroup->setEnabled(enabled);

    // Current limit
    uint8_t currentLimit;
    des.readRaw<uint8_t>(&currentLimit);
    m_nodeGroup->setCurrentLimit(currentLimit);

    uint8_t alwaysRetry;
    des.readRaw<uint8_t>(&alwaysRetry);
    m_nodeGroup->setAlwaysRetry(alwaysRetry);

    uint8_t maxRetries;
    des.readRaw<uint8_t>(&maxRetries);
    m_nodeGroup->setMaxRetries(maxRetries);

    float retryDelay;
    des.readRaw<float>(&retryDelay);
    m_nodeGroup->setRetryDelay(retryDelay);

    // Nodes
    {
        uint16_t nodeCount;
        des.readRaw<uint16_t>(&nodeCount);
        for (int i = 0; i < nodeCount; i++) {
            std::string name;
            des.readRaw<std::string>(&name);

            uint16_t tempType;
            des.readRaw<uint16_t>(&tempType);
            NodeTypes type = (NodeTypes)tempType;

            float x, y;
            des.readRaw<float>(&x);
            des.readRaw<float>(&y);

            auto newNode = m_nodeGroup->addNode(type, { x, y });
            newNode->setName(name);

            // Inputs - skip till later
            uint8_t inputCount;
            des.readRaw<uint8_t>(&inputCount);
            for (int j = 0; j < inputCount; j++) {
                int8_t temp;
                des.readRaw<int8_t>(&temp); // Connection type
            }

            // Values
            uint8_t valueCount;
            des.readRaw<uint8_t>(&valueCount);
            for (int valueIndex = 0; valueIndex < valueCount; valueIndex++) {
                float value;
                des.readRaw<float>(&value);
                newNode->setValue(valueIndex, value);
            }

            // Special Node Groups
            if (type == NodeTypes::Node_Group) {
                uint64_t uuid;
                des.readRaw<uint64_t>(&uuid);
                newNode->setSelectedNodeGroup(uuid);
                // Not used because we will look up by UUID

                uint8_t address, index;
                des.readRaw<uint8_t>(&address); // Module address
                des.readRaw<uint8_t>(&index); // Pin index
            }
        }
    }

    // Connect inputs to already added nodes
    {
        uint16_t nodeCount;
        des.readRaw<uint16_t>(&nodeCount);
        for (int nodeIndex = 0; nodeIndex < nodeCount; nodeIndex++) {

            // Inputs
            uint8_t inputCount;
            des.readRaw<uint8_t>(&inputCount);
            for (int inputIndex = 0; inputIndex < inputCount; inputIndex++) {
                int8_t connectionNodeIndex;
                des.readRaw<int8_t>(&connectionNodeIndex);

                int8_t outputIndex;
                des.readRaw<int8_t>(&outputIndex);

                float defaultValue;
                des.readRaw<float>(&defaultValue);
                if (connectionNodeIndex > -1 && outputIndex > -1) {
                    // Has connection
                    NodeConnection nodeConnection = { m_nodeGroup->getNodes()[connectionNodeIndex], outputIndex, defaultValue };
                    m_nodeGroup->getNodes()[nodeIndex]->setInput(inputIndex, nodeConnection);
                } else {
                    m_nodeGroup->getNodes()[nodeIndex]->getInput(inputIndex).DefaultValue = defaultValue;
                }
            }
        }
    }

    return true;
}