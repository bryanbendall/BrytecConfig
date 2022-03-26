#include "NodeGroupNode.h"
#include "BrytecConfigEmbedded/Nodes/ENodeGroupNode.h"

NodeGroupNode::NodeGroupNode(int id, ImVec2 position, NodeTypes type)
    : Node(id, position, type)
{
    m_outputs.push_back(0.0f);
    m_values.push_back(0.0f);
}

void NodeGroupNode::evaluate()
{
    m_outputs[0] = m_values[0];
}
