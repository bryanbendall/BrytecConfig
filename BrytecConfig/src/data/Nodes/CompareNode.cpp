#include "CompareNode.h"
#include "BrytecConfigEmbedded/Nodes/ECompareNode.h"

CompareNode::CompareNode(int id, ImVec2 position, NodeTypes type)
    : Node(id, position, type)
{
    m_inputs.push_back(NodeConnection()); // Value 1
    m_inputs.push_back(NodeConnection()); // Value 2
    m_outputs.push_back(0.0f); // Output
    m_values.push_back(0.0f); // Compare type
}

void CompareNode::evaluate()
{
    ECompareNode node;

    node.SetValue(0, getInputValue(0));
    node.SetValue(1, getInputValue(1));
    node.SetValue(2, getValue(0));

    node.Evaluate(ImGui::GetIO().DeltaTime);
    m_outputs[0] = node.m_out;
}
