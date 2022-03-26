#include "MathNode.h"
#include "BrytecConfigEmbedded/Nodes/EMathNode.h"

MathNode::MathNode(int id, ImVec2 position, NodeTypes type)
    : Node(id, position, type)
{
    m_inputs.push_back(NodeConnection());
    m_inputs.push_back(NodeConnection());
    m_outputs.push_back(0.0f);
    m_values.push_back(0.0f);
}

void MathNode::evaluate()
{
    EMathNode node;

    node.SetValue(0, getInputValue(0));
    node.SetValue(1, getInputValue(1));
    node.SetValue(2, getValue(0));

    node.Evaluate(ImGui::GetIO().DeltaTime);
    m_outputs[0] = node.m_out;
}
