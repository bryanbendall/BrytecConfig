#include "ConvertNode.h"
#include "BrytecConfigEmbedded/Nodes/EConvertNode.h"

ConvertNode::ConvertNode(int id, ImVec2 position, NodeTypes type)
    : Node(id, position, type)
{
    m_inputs.push_back(NodeConnection());
    m_inputs.push_back(NodeConnection());
    m_inputs.push_back(NodeConnection());
    m_inputs.push_back(NodeConnection());
    m_outputs.push_back(0.0f);
    m_values.push_back(0.0f);
    m_values.push_back(0.0f);
}

void ConvertNode::evaluate()
{
}
