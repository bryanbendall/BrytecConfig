#include "OnOffNode.h"
#include "BrytecConfigEmbedded/Nodes/EOnOffNode.h"

OnOffNode::OnOffNode(int id, ImVec2 position, NodeTypes type)
    : Node(id, position, type)
{
    m_inputs.push_back(NodeConnection()); // On
    m_inputs.push_back(NodeConnection()); // Off
    m_outputs.push_back(0.0f); // Output
}

void OnOffNode::evaluate()
{
    EOnOffNode node;

    node.SetValue(0, getInputValue(0));
    node.SetValue(1, getInputValue(1));

    node.Evaluate(ImGui::GetIO().DeltaTime);
    m_outputs[0] = node.m_out;
}
