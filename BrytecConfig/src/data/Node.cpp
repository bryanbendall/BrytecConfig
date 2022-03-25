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

const char* Node::s_compareNames[(int)CompareNode::Types::Count] = {
    "Greater Than",
    "Greater Equal To",
    "Less Than",
    "Less Equal To"
};

const char* Node::s_mathNames[(int)MathNode::Types::Count] = {
    "Add",
    "Subtract",
    "Multiply",
    "Divide"
};

const char* Node::s_curveNames[(int)CurveNode::Types::Count] = {
    "Toggle",
    "Linear",
    "Expontial",
    "Breathing"
};

Node::Node(int id, ImVec2 position, NodeTypes type)
    : m_id(id)
    , m_position(position)
    , m_type(type)
    , m_name("")
{

    switch (type) {
    case NodeTypes::Initial_Value:
        m_outputs.push_back(0.0f);
        m_values.push_back(0.0f);
        break;
    case NodeTypes::Final_Value:
        m_inputs.push_back(NodeConnection());
        break;
    case NodeTypes::Node_Group:
        m_outputs.push_back(0.0f);
        m_values.push_back(0.0f);
        break;
    case NodeTypes::And:
        m_inputs.push_back(NodeConnection(1.0f)); // Input
        m_inputs.push_back(NodeConnection(1.0f)); // Input
        m_inputs.push_back(NodeConnection(1.0f)); // Input
        m_inputs.push_back(NodeConnection(1.0f)); // Input
        m_inputs.push_back(NodeConnection(1.0f)); // Input
        m_outputs.push_back(0.0f); // Output
        break;
    case NodeTypes::Or:
        m_inputs.push_back(NodeConnection()); // Input
        m_inputs.push_back(NodeConnection()); // Input
        m_inputs.push_back(NodeConnection()); // Input
        m_inputs.push_back(NodeConnection()); // Input
        m_inputs.push_back(NodeConnection()); // Input
        m_outputs.push_back(0.0f); // Output
        break;
    case NodeTypes::Two_Stage:
        m_inputs.push_back(NodeConnection()); // Stage 1 trig
        m_inputs.push_back(NodeConnection(50.0f)); // Stage 1 value
        m_inputs.push_back(NodeConnection()); // Stage 2 trig
        m_inputs.push_back(NodeConnection(100.0f)); // Stage 2 value
        m_outputs.push_back(0.0f); // Output
        break;
    case NodeTypes::Curve:
        m_inputs.push_back(NodeConnection()); // Input
        m_inputs.push_back(NodeConnection()); // Repeat
        m_inputs.push_back(NodeConnection()); // Time
        m_outputs.push_back(0.0f); // Output
        m_values.push_back(0.0f); // Curve type
        m_values.push_back(0.0f); // Internet time counter
        break;
    case NodeTypes::Compare:
        m_inputs.push_back(NodeConnection()); // Value 1
        m_inputs.push_back(NodeConnection()); // Value 2
        m_outputs.push_back(0.0f); // Output
        m_values.push_back(0.0f); // Compare type
        break;
    case NodeTypes::On_Off:
        m_inputs.push_back(NodeConnection()); // On
        m_inputs.push_back(NodeConnection()); // Off
        m_outputs.push_back(0.0f); // Output
        break;
    case NodeTypes::Invert:
        m_inputs.push_back(NodeConnection()); // Input
        m_outputs.push_back(0.0f); // Output
        break;
    case NodeTypes::Toggle:
        m_inputs.push_back(NodeConnection()); // Input
        m_outputs.push_back(0.0f); // Output
        m_values.push_back(0.0f); // Internal last value
        break;
    case NodeTypes::Delay:
        m_inputs.push_back(NodeConnection()); // Input
        m_inputs.push_back(NodeConnection(1.0f)); // Delay time
        m_outputs.push_back(0.0f); // Output
        m_values.push_back(0.0f); // Internal timer
        break;
    case NodeTypes::Push_Button:
        m_inputs.push_back(NodeConnection()); // Button
        m_inputs.push_back(NodeConnection()); // Neutral Safety
        m_inputs.push_back(NodeConnection()); // Engine running
        m_outputs.push_back(0.0f); // Ignition output
        m_outputs.push_back(0.0f); // Starter output
        m_values.push_back(0.0f); // Internal state
        break;
    case NodeTypes::Map_Value:
        m_inputs.push_back(NodeConnection()); // Value
        m_inputs.push_back(NodeConnection()); // From min
        m_inputs.push_back(NodeConnection(1.0f)); // From max
        m_inputs.push_back(NodeConnection()); // To min
        m_inputs.push_back(NodeConnection(100.0f)); // To max
        m_outputs.push_back(0.0f); // Output
        break;
    case NodeTypes::Math:
        m_inputs.push_back(NodeConnection());
        m_inputs.push_back(NodeConnection());
        m_outputs.push_back(0.0f);
        m_values.push_back(0.0f);
        break;
    case NodeTypes::Value:
        m_outputs.push_back(0.0f);
        m_values.push_back(0.0f);
        break;
    case NodeTypes::Switch:
        m_inputs.push_back(NodeConnection());
        m_inputs.push_back(NodeConnection());
        m_inputs.push_back(NodeConnection());
        m_outputs.push_back(0.0f);
        break;
    case NodeTypes::CanBus:
        m_outputs.push_back(0.0f);
        m_outputs.push_back(0.0f);
        m_outputs.push_back(0.0f);
        m_outputs.push_back(0.0f);
        m_outputs.push_back(0.0f);
        m_outputs.push_back(0.0f);
        m_outputs.push_back(0.0f);
        m_outputs.push_back(0.0f);
        m_outputs.push_back(0.0f);
        break;
    case NodeTypes::Count:
        assert(false);
        break;
    default:
        assert(false);
        break;
    }
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

void Node::evaluate()
{
    switch (m_type) {
    case NodeTypes::Initial_Value:
        m_outputs[0] = m_values[0];
        break;
    case NodeTypes::Final_Value:
        break;
    case NodeTypes::Node_Group:
        m_outputs[0] = m_values[0];
        break;
    case NodeTypes::And:
        evaluateAnd();
        break;
    case NodeTypes::Or:
        evaluateOr();
        break;
    case NodeTypes::Two_Stage:
        evaluateTwoStage();
        break;
    case NodeTypes::Curve:
        evaulateCurve();
        break;
    case NodeTypes::Compare:
        evaluateCompare();
        break;
    case NodeTypes::On_Off:
        evaluateOnOff();
        break;
    case NodeTypes::Invert:
        evaluateInvert();
        break;
    case NodeTypes::Toggle:
        evaluateToggle();
        break;
    case NodeTypes::Delay:
        evaluateDelay();
        break;
    case NodeTypes::Push_Button:
        evaluatePushButton();
        break;
    case NodeTypes::Map_Value:
        evaluateMap();
        break;
    case NodeTypes::Math:
        evaluateMath();
        break;
    case NodeTypes::Value:
        m_outputs[0] = m_values[0];
        break;
    case NodeTypes::Switch:
        evaluateSwitch();
        break;
    case NodeTypes::Count:
        break;
    default:
        assert(false);
        break;
    }
}

void Node::evaluateAnd()
{
    // AndNode node;
    // NodeInputMask mask;

    // node.SetValue(0, mask, getInputValue(0));
    // node.SetValue(1, mask, getInputValue(1));
    // node.SetValue(2, mask, getInputValue(2));
    // node.SetValue(3, mask, getInputValue(3));
    // node.SetValue(4, mask, getInputValue(4));

    // node.Evaluate(mask, ImGui::GetIO().DeltaTime);
    // m_outputs[0] = node.m_out;
}

void Node::evaluateOr()
{
    OrNode node;
    NodeInputMask mask;

    node.SetValue(0, mask, getInputValue(0));
    node.SetValue(1, mask, getInputValue(1));
    node.SetValue(2, mask, getInputValue(2));
    node.SetValue(3, mask, getInputValue(3));
    node.SetValue(4, mask, getInputValue(4));

    node.Evaluate(mask, ImGui::GetIO().DeltaTime);
    m_outputs[0] = node.m_out;
}

void Node::evaluateInvert()
{
    InvertNode node;
    NodeInputMask mask;

    node.SetValue(0, mask, getInputValue(0));

    node.Evaluate(mask, ImGui::GetIO().DeltaTime);
    m_outputs[0] = node.m_out;
}

void Node::evaluateTwoStage()
{
    TwoStageNode node;
    NodeInputMask mask;

    node.SetValue(0, mask, getInputValue(0));
    node.SetValue(1, mask, getInputValue(1));
    node.SetValue(2, mask, getInputValue(2));
    node.SetValue(3, mask, getInputValue(3));

    node.Evaluate(mask, ImGui::GetIO().DeltaTime);
    m_outputs[0] = node.m_out;
}

void Node::evaluateOnOff()
{
    OnOffNode node;
    NodeInputMask mask;

    node.SetValue(0, mask, getInputValue(0));
    node.SetValue(1, mask, getInputValue(1));

    node.Evaluate(mask, ImGui::GetIO().DeltaTime);
    m_outputs[0] = node.m_out;
}

void Node::evaluateToggle()
{
    ToggleNode node;
    NodeInputMask mask;

    node.SetValue(0, mask, getInputValue(0));
    node.SetValue(1, mask, getValue(0));
    node.m_out = m_outputs[0];

    node.Evaluate(mask, ImGui::GetIO().DeltaTime);
    m_values[0] = node.m_lastValue.value;
    m_outputs[0] = node.m_out;
}

void Node::evaluateDelay()
{
    DelayNode node;
    NodeInputMask mask;

    node.SetValue(0, mask, getInputValue(0));
    node.SetValue(1, mask, getInputValue(1));
    node.SetValue(2, mask, getValue(0));

    node.Evaluate(mask, ImGui::GetIO().DeltaTime);
    m_values[0] = node.m_counter.value;
    m_outputs[0] = node.m_out;
}

void Node::evaluateCompare()
{
    CompareNode node;
    NodeInputMask mask;

    node.SetValue(0, mask, getInputValue(0));
    node.SetValue(1, mask, getInputValue(1));
    node.SetValue(2, mask, getValue(0));

    node.Evaluate(mask, ImGui::GetIO().DeltaTime);
    m_outputs[0] = node.m_out;
}

void Node::evaluateMath()
{
    MathNode node;
    NodeInputMask mask;

    node.SetValue(0, mask, getInputValue(0));
    node.SetValue(1, mask, getInputValue(1));
    node.SetValue(2, mask, getValue(0));

    node.Evaluate(mask, ImGui::GetIO().DeltaTime);
    m_outputs[0] = node.m_out;
}

void Node::evaluateMap()
{
    MapValueNode node;
    NodeInputMask mask;

    node.SetValue(0, mask, getInputValue(0));
    node.SetValue(1, mask, getInputValue(1));
    node.SetValue(2, mask, getInputValue(2));
    node.SetValue(3, mask, getInputValue(3));
    node.SetValue(4, mask, getInputValue(4));

    node.Evaluate(mask, ImGui::GetIO().DeltaTime);
    m_outputs[0] = node.m_out;
}

void Node::evaulateCurve()
{
    CurveNode node;
    NodeInputMask mask;

    node.SetValue(0, mask, getInputValue(0));
    node.SetValue(1, mask, getInputValue(1));
    node.SetValue(2, mask, getInputValue(2));
    node.SetValue(3, mask, getValue(0));
    node.SetValue(4, mask, getValue(1));

    node.Evaluate(mask, ImGui::GetIO().DeltaTime);
    m_values[1] = node.m_timerCounter.value;
    m_outputs[0] = node.m_out;
}

void Node::evaluatePushButton()
{
    PushButtonNode node;
    NodeInputMask mask;

    node.SetValue(0, mask, getInputValue(0));
    node.SetValue(1, mask, getInputValue(1));
    node.SetValue(2, mask, getInputValue(2));
    node.SetValue(3, mask, getValue(0));
    node.m_ignitionOut = m_outputs[0];
    node.m_starterOut = m_outputs[1];

    node.Evaluate(mask, ImGui::GetIO().DeltaTime);
    m_values[0] = node.m_lastButtonState.value;
    m_outputs[0] = node.m_ignitionOut;
    m_outputs[1] = node.m_starterOut;
}

void Node::evaluateSwitch()
{
    SwitchNode node;
    NodeInputMask mask;

    node.SetValue(0, mask, getInputValue(0));
    node.SetValue(1, mask, getInputValue(1));
    node.SetValue(2, mask, getInputValue(2));
    node.m_out = m_outputs[0];

    node.Evaluate(mask, ImGui::GetIO().DeltaTime);
    m_outputs[0] = node.m_out;
}

bool Node::hasConnection(int inputIndex)
{
    return !m_inputs[inputIndex].ConnectedNode.expired();
}
