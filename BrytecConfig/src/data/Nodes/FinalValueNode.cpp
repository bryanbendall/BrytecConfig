#include "FinalValueNode.h"
#include "BrytecConfigEmbedded/Nodes/EValueNode.h"

FinalValueNode::FinalValueNode(int id, ImVec2 position, NodeTypes type)
    : Node(id, position, type)
{
    m_inputs.push_back(NodeConnection());
}

void FinalValueNode::evaluate()
{
    // Nothing to do
}
