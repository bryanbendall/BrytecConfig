#include "PushButtonNode.h"
#include "BrytecConfigEmbedded/Nodes/EPushButtonNode.h"

PushButtonNode::PushButtonNode(int id, ImVec2 position, NodeTypes type)
    : Node(id, position, type)
{
    m_inputs.push_back(NodeConnection()); // Button
    m_inputs.push_back(NodeConnection()); // Neutral Safety
    m_inputs.push_back(NodeConnection()); // Engine running
    m_outputs.push_back(0.0f); // Ignition output
    m_outputs.push_back(0.0f); // Starter output
    m_values.push_back(0.0f); // Internal state
}

void PushButtonNode::evaluate()
{
    // EPushButtonNode node;

    // node.SetValue(0, getInputValue(0));
    // node.SetValue(1, getInputValue(1));
    // node.SetValue(2, getInputValue(2));
    // node.SetValue(3, getValue(0));
    // node.m_ignitionOut = m_outputs[0];
    // node.m_starterOut = m_outputs[1];

    // node.Evaluate(ImGui::GetIO().DeltaTime);
    // m_values[0] = node.m_lastButtonState.value;
    // m_outputs[0] = node.m_ignitionOut;
    // m_outputs[1] = node.m_starterOut;
}
