#include "ToggleNode.h"
#include "BrytecConfigEmbedded/Nodes/EToggleNode.h"

ToggleNode::ToggleNode(int id, ImVec2 position, NodeTypes type)
    : Node(id, position, type)
{
    m_inputs.push_back(NodeConnection()); // Input
    m_outputs.push_back(0.0f); // Output
    m_values.push_back(0.0f); // Internal last value
}

void ToggleNode::evaluate()
{
    EToggleNode node;

    node.SetValue(0, getInputValue(0));
    node.SetValue(1, getValue(0));
    node.m_out = m_outputs[0];

    node.Evaluate(ImGui::GetIO().DeltaTime);
    m_values[0] = node.m_lastValue.value;
    m_outputs[0] = node.m_out;
}
