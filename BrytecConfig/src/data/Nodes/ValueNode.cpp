#include "ValueNode.h"
#include "BrytecConfigEmbedded/Nodes/EValueNode.h"

ValueNode::ValueNode(int id, ImVec2 position, NodeTypes type)
    : Node(id, position, type)
{
    m_outputs.push_back(0.0f);
    m_values.push_back(0.0f);
}

void ValueNode::evaluate()
{
    m_outputs[0] = m_values[0];
}
