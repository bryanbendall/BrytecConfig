#include "InvertNode.h"
#include "BrytecConfigEmbedded/Nodes/EInvertNode.h"

InvertNode::InvertNode(int id, ImVec2 position, NodeTypes type)
    : Node(id, position, type)
{
    m_inputs.push_back(NodeConnection()); // Input
    m_outputs.push_back(0.0f); // Output
}

void InvertNode::evaluate()
{
    EInvertNode node;

    node.SetValue(0, getInputValue(0));

    node.Evaluate(ImGui::GetIO().DeltaTime);
    m_outputs[0] = node.m_out;
}
