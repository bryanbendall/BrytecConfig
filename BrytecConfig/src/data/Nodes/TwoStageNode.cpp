#include "TwoStageNode.h"
#include "BrytecConfigEmbedded/Nodes/ETwoStageNode.h"

TwoStageNode::TwoStageNode(int id, ImVec2 position, NodeTypes type)
    : Node(id, position, type)
{
    m_inputs.push_back(NodeConnection()); // Stage 1 trig
    m_inputs.push_back(NodeConnection(50.0f)); // Stage 1 value
    m_inputs.push_back(NodeConnection()); // Stage 2 trig
    m_inputs.push_back(NodeConnection(100.0f)); // Stage 2 value
    m_outputs.push_back(0.0f); // Output
}

void TwoStageNode::evaluate()
{
    ETwoStageNode node;

    node.SetValue(0, getInputValue(0));
    node.SetValue(1, getInputValue(1));
    node.SetValue(2, getInputValue(2));
    node.SetValue(3, getInputValue(3));

    node.Evaluate(ImGui::GetIO().DeltaTime);
    m_outputs[0] = node.m_out;
}
