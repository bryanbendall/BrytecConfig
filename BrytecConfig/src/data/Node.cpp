#include "Node.h"
#include "NodeGroup.h"
#include "Nodes/AndNode.h"
#include <iostream>

const char* Node::s_nodeName[(int)NodeTypes::Count] = {
    "Final Value",
    "Initial Value",
    "Node Group",
    "And",
    "Or",
    "Two Stage",
    "Curve",
    "Compare",
    "On/Off",
    "Invert",
    "Toggle",
    "Delay",
    "Push Button Start",
    "Map Value",
    "Math",
    "Value",
    "Switch",
    "Can Bus"
};

Node::Node(int id, ImVec2 position, NodeTypes type)
    : m_id(id)
    , m_position(position)
    , m_type(type)
    , m_name("")
{
}

std::shared_ptr<Node> Node::create(int id, ImVec2 position, NodeTypes type)
{
    switch (type) {
    case NodeTypes::And:
        return std::make_shared<AndNode>(id, position, type);

    default:
        assert(false);
        return nullptr;
    }
}

float Node::getOutputValue(size_t outputIndex)
{
    if (outputIndex >= m_outputs.size())
        return 0.0f;

    return m_outputs[outputIndex];
}

void Node::setInput(int inputIndex, NodeConnection nodeConnection)
{
    assert(!(inputIndex > (int)m_inputs.size()));
    m_inputs[inputIndex] = nodeConnection;
}

float& Node::getInputValue(int inputIndex)
{
    if (hasConnection(inputIndex))
        return m_inputs[inputIndex].ConnectedNode.lock()->m_outputs[m_inputs[inputIndex].OutputIndex];
    else
        return m_inputs[inputIndex].DefaultValue;
}

unsigned int Node::getBytesSize()
{
    return 0;
}

bool Node::hasConnection(int inputIndex)
{
    return !m_inputs[inputIndex].ConnectedNode.expired();
}
