#include "Node.h"
#include "NodeGroup.h"
#include <iostream>

namespace Brytec {

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
    "Push Button Start (Obsolete)",
    "Map Value",
    "Math",
    "Value",
    "Switch",
    "Can Bus Input",
    "Convert (Obsolete)",
    "Pin Current",
    "PID (Experamental)",
    "Counter",
    "Color",
    "Racepak Switch Panel (Experamental)",
    "Holley Broadcast (Experamental)",
    "Interpolate",
    "Can Bus Output",
    "Thermistor"
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
        m_inputs.push_back(NodeConnection(0.0f)); // Default Value
        m_inputs.push_back(NodeConnection(0.25f)); // Timeout
        m_outputs.push_back(0.0f); // Output
        m_values.push_back(0.0f); // Used for simulation
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
        m_inputs.push_back(NodeConnection(1.0f)); // Time
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
        m_inputs.push_back(NodeConnection()); // Reset
        m_outputs.push_back(0.0f); // Output
        m_values.push_back(0.0f); // Internal last input value
        m_values.push_back(0.0f); // Internal last output value
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
        m_values.push_back(0.0f); // Last button state
        m_values.push_back(0.0f); // Ignition out state
        m_values.push_back(0.0f); // Starter out state
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

    case NodeTypes::CanBusInput:
        m_values.push_back(1.0f); // Id
        m_values.push_back(0.0f); // Can index
        m_values.push_back(0.0f); // Endian
        m_values.push_back(0.0f); // Data Type
        m_values.push_back(0.0f); // Start Byte
        m_outputs.push_back(0.0f);
        break;

    case NodeTypes::Convert:
        m_inputs.push_back(NodeConnection());
        m_inputs.push_back(NodeConnection());
        m_inputs.push_back(NodeConnection());
        m_inputs.push_back(NodeConnection());
        m_values.push_back(0.0f);
        m_values.push_back(0.0f);
        m_outputs.push_back(0.0f);
        break;

    case NodeTypes::PinCurrent:
        m_values.push_back(0.0f);
        m_outputs.push_back(0.0f);
        break;

    case NodeTypes::PID:
        m_inputs.push_back(NodeConnection()); // Input
        m_inputs.push_back(NodeConnection()); // Target
        m_inputs.push_back(NodeConnection()); // P
        m_inputs.push_back(NodeConnection()); // I
        m_inputs.push_back(NodeConnection()); // D
        m_values.push_back(0.0f); // previous error
        m_values.push_back(0.0f); // integral
        m_outputs.push_back(0.0f);
        break;

    case NodeTypes::Counter:
        m_inputs.push_back(NodeConnection()); // Up
        m_inputs.push_back(NodeConnection()); // Down
        m_inputs.push_back(NodeConnection()); // Min
        m_inputs.push_back(NodeConnection(1.0f)); // Max
        m_inputs.push_back(NodeConnection()); // Loop
        m_outputs.push_back(0.0f);
        m_values.push_back(0.0f); // Last up
        m_values.push_back(0.0f); // Last down
        m_values.push_back(0.0f); // Output value
        break;

    case NodeTypes::Color:
        m_inputs.push_back(NodeConnection()); // R
        m_inputs.push_back(NodeConnection()); // G
        m_inputs.push_back(NodeConnection()); // B
        m_outputs.push_back(0.0f);
        break;

    case NodeTypes::Racepak_Switch_Panel:
        m_inputs.push_back(NodeConnection());
        m_inputs.push_back(NodeConnection());
        m_inputs.push_back(NodeConnection());
        m_inputs.push_back(NodeConnection());
        m_inputs.push_back(NodeConnection());
        m_inputs.push_back(NodeConnection());
        m_inputs.push_back(NodeConnection());
        m_inputs.push_back(NodeConnection());
        break;

    case NodeTypes::Holley_Broadcast:
        m_outputs.push_back(0.0f); // Output value
        m_values.push_back(0.0f); // Data channel
        break;

    case NodeTypes::Interpolate:
        m_inputs.push_back(NodeConnection()); // Input
        m_inputs.push_back(NodeConnection(1.0f)); // Transition time
        m_outputs.push_back(0.0f); // Output
        m_values.push_back(0.0f); // Interpolate type
        m_values.push_back(0.0f); // Internal timer
        m_values.push_back(0.0f); // Output
        break;

    case NodeTypes::CanBusOutput:
        m_inputs.push_back(0.0f); // Data
        m_inputs.push_back(0.0f); // Send?
        m_values.push_back(1.0f); // Id
        m_values.push_back(0.0f); // Can index
        m_values.push_back(0.0f); // Frame type
        m_values.push_back(0.0f); // Endian
        m_values.push_back(0.0f); // Data Type
        m_values.push_back(0.0f); // Start Byte
        m_values.push_back(0.0f); // Last Send
        break;

    case NodeTypes::Thermistor:
        m_inputs.push_back(0.0f); // Input
        m_values.push_back(0.0f); // Res 1
        m_values.push_back(0.0f); // Temp 1
        m_values.push_back(0.0f); // Res 2
        m_values.push_back(0.0f); // Temp 2
        m_values.push_back(0.0f); // Temp Format
        m_values.push_back(0.0f); // Beta
        m_outputs.push_back(0.0f); // Output
        break;

    case NodeTypes::Count:
    default:
        assert(false);
        break;
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

}