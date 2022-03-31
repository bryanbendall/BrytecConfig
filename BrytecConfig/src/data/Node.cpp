#include "Node.h"
#include "NodeGroup.h"
#include "Nodes/AndNode.h"
#include "Nodes/CanBusNode.h"
#include "Nodes/CompareNode.h"
#include "Nodes/ConvertNode.h"
#include "Nodes/CurveNode.h"
#include "Nodes/DelayNode.h"
#include "Nodes/FinalValueNode.h"
#include "Nodes/InitialValueNode.h"
#include "Nodes/InvertNode.h"
#include "Nodes/MapValueNode.h"
#include "Nodes/MathNode.h"
#include "Nodes/NodeGroupNode.h"
#include "Nodes/OnOffNode.h"
#include "Nodes/OrNode.h"
#include "Nodes/PushButtonNode.h"
#include "Nodes/SwitchNode.h"
#include "Nodes/ToggleNode.h"
#include "Nodes/TwoStageNode.h"
#include "Nodes/ValueNode.h"
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
    "Can Bus",
    "Convert"
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
    case NodeTypes::Final_Value:
        return std::make_shared<FinalValueNode>(id, position, type);
    case NodeTypes::Initial_Value:
        return std::make_shared<InitialValueNode>(id, position, type);
    case NodeTypes::Node_Group:
        return std::make_shared<NodeGroupNode>(id, position, type);
    case NodeTypes::And:
        return std::make_shared<AndNode>(id, position, type);
    case NodeTypes::Or:
        return std::make_shared<OrNode>(id, position, type);
    case NodeTypes::Two_Stage:
        return std::make_shared<TwoStageNode>(id, position, type);
    case NodeTypes::Curve:
        return std::make_shared<CurveNode>(id, position, type);
    case NodeTypes::Compare:
        return std::make_shared<CompareNode>(id, position, type);
    case NodeTypes::On_Off:
        return std::make_shared<OnOffNode>(id, position, type);
    case NodeTypes::Invert:
        return std::make_shared<InvertNode>(id, position, type);
    case NodeTypes::Toggle:
        return std::make_shared<ToggleNode>(id, position, type);
    case NodeTypes::Delay:
        return std::make_shared<DelayNode>(id, position, type);
    case NodeTypes::Push_Button:
        return std::make_shared<PushButtonNode>(id, position, type);
    case NodeTypes::Map_Value:
        return std::make_shared<MapValueNode>(id, position, type);
    case NodeTypes::Math:
        return std::make_shared<MathNode>(id, position, type);
    case NodeTypes::Value:
        return std::make_shared<ValueNode>(id, position, type);
    case NodeTypes::Switch:
        return std::make_shared<SwitchNode>(id, position, type);
    case NodeTypes::CanBus:
        return std::make_shared<CanBusNode>(id, position, type);
    case NodeTypes::Convert:
        return std::make_shared<ConvertNode>(id, position, type);

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
