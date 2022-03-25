#include "AndNode.h"
#include "BrytecConfigEmbedded/Nodes/EAndNode.h"

AndNode::AndNode(int id, ImVec2 position, NodeTypes type)
    : Node(id, position, type)
{
    m_inputs.push_back(NodeConnection(1.0f)); // Input
    m_inputs.push_back(NodeConnection(1.0f)); // Input
    m_inputs.push_back(NodeConnection(1.0f)); // Input
    m_inputs.push_back(NodeConnection(1.0f)); // Input
    m_inputs.push_back(NodeConnection(1.0f)); // Input
    m_outputs.push_back(0.0f); // Output
}

void AndNode::evaluate()
{
    EAndNode node;

    node.SetValue(0, getInputValue(0));
    node.SetValue(1, getInputValue(1));
    node.SetValue(2, getInputValue(2));
    node.SetValue(3, getInputValue(3));
    node.SetValue(4, getInputValue(4));

    node.Evaluate(ImGui::GetIO().DeltaTime);
    m_outputs[0] = *node.GetOutput(0); // node.m_out;
}
