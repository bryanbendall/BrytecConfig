#include "SwitchNode.h"
#include "BrytecConfigEmbedded/Nodes/ESwitchNode.h"

SwitchNode::SwitchNode(int id, ImVec2 position, NodeTypes type)
    : Node(id, position, type)
{
    m_inputs.push_back(NodeConnection());
    m_inputs.push_back(NodeConnection());
    m_inputs.push_back(NodeConnection());
    m_outputs.push_back(0.0f);
}

void SwitchNode::evaluate()
{
    ESwitchNode node;

    node.SetValue(0, getInputValue(0));
    node.SetValue(1, getInputValue(1));
    node.SetValue(2, getInputValue(2));
    node.m_out = m_outputs[0];

    node.Evaluate(ImGui::GetIO().DeltaTime);
    m_outputs[0] = node.m_out;
}
