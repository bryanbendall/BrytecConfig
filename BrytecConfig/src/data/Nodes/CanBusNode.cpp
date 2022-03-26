#include "CanBusNode.h"
#include "BrytecConfigEmbedded/Nodes/ECanBusNode.h"

CanBusNode::CanBusNode(int id, ImVec2 position, NodeTypes type)
    : Node(id, position, type)
{
    m_outputs.push_back(0.0f);
    m_outputs.push_back(0.0f);
    m_outputs.push_back(0.0f);
    m_outputs.push_back(0.0f);
    m_outputs.push_back(0.0f);
    m_outputs.push_back(0.0f);
    m_outputs.push_back(0.0f);
    m_outputs.push_back(0.0f);
    m_outputs.push_back(0.0f);
}

void CanBusNode::evaluate()
{
    ECanBusNode node;

    // Do something
}
