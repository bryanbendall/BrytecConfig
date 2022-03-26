#include "DelayNode.h"
#include "BrytecConfigEmbedded/Nodes/EDelayNode.h"

DelayNode::DelayNode(int id, ImVec2 position, NodeTypes type)
    : Node(id, position, type)
{
    m_inputs.push_back(NodeConnection()); // Input
    m_inputs.push_back(NodeConnection(1.0f)); // Delay time
    m_outputs.push_back(0.0f); // Output
    m_values.push_back(0.0f); // Internal timer
}

void DelayNode::evaluate()
{
    EDelayNode node;

    node.SetValue(0, getInputValue(0));
    node.SetValue(1, getInputValue(1));
    node.SetValue(2, getValue(0));

    node.Evaluate(ImGui::GetIO().DeltaTime);
    m_values[0] = node.m_counter.value;
    m_outputs[0] = node.m_out;
}
