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
    des.readRaw<char>(); // N
    des.readRaw<char>(); // G

    // Version
    des.readRaw<uint8_t>(); // Major
    des.readRaw<uint8_t>(); // Minor

    // Basic info
    m_nodeGroup->setName(des.readRaw<std::string>());
    m_nodeGroup->setId(des.readRaw<uint64_t>());
    m_nodeGroup->setType((IOTypes::Types)des.readRaw<uint8_t>());
    m_nodeGroup->setEnabled(des.readRaw<uint8_t>());

    {
        uint16_t nodeCount = des.readRaw<uint16_t>();
        for (int i = 0; i < nodeCount; i++) {
            std::string name = des.readRaw<std::string>();
            NodeTypes type = (NodeTypes)des.readRaw<uint16_t>();
            float x = des.readRaw<float>();
            float y = des.readRaw<float>();

            auto newNode = m_nodeGroup->addNode(type, { x, y });
            newNode->setName(name);

            // Inputs - skip till later
            uint8_t inputCount = des.readRaw<uint8_t>();
            for (int j = 0; j < inputCount; j++) {
                des.readRaw<int8_t>(); // Connection type
            }

            // Values
            uint8_t valueCount = des.readRaw<uint8_t>();
            for (int valueIndex = 0; valueIndex < valueCount; valueIndex++)
                newNode->setValue(valueIndex, des.readRaw<float>());

            // Special Node Groups
            if (type == NodeTypes::Node_Group) {
                newNode->setSelectedNodeGroup(des.readRaw<uint64_t>());
                // Not used because we will look up by UUID
                des.readRaw<uint8_t>(); // Module address
                des.readRaw<uint8_t>(); // Pin index
            }
        }
    }

    // Connect inputs to already added nodes
    {
        uint16_t nodeCount = des.readRaw<uint16_t>();
        for (int nodeIndex = 0; nodeIndex < nodeCount; nodeIndex++) {

            // Inputs
            uint8_t inputCount = des.readRaw<uint8_t>();
            for (int inputIndex = 0; inputIndex < inputCount; inputIndex++) {
                auto connectionNodeIndex = des.readRaw<int8_t>();
                auto outputIndex = des.readRaw<int8_t>();
                auto defaultValue = des.readRaw<float>();
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