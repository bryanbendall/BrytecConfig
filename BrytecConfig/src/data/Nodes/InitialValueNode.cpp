#include "InitialValueNode.h"
#include "BrytecConfigEmbedded/Nodes/EInitialValueNode.h"

InitialValueNode::InitialValueNode(int id, ImVec2 position, NodeTypes type)
    : Node(id, position, type)
{
    m_outputs.push_back(0.0f);
    m_values.push_back(0.0f);
}

void InitialValueNode::evaluate()
{
    m_outputs[0] = m_values[0];
}
