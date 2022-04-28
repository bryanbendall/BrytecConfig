#include "CurveNode.h"
#include "BrytecConfigEmbedded/Nodes/ECurveNode.h"

CurveNode::CurveNode(int id, ImVec2 position, NodeTypes type)
    : Node(id, position, type)
{
    m_inputs.push_back(NodeConnection()); // Input
    m_inputs.push_back(NodeConnection()); // Repeat
    m_inputs.push_back(NodeConnection()); // Time
    m_outputs.push_back(0.0f); // Output
    m_values.push_back(0.0f); // Curve type
    m_values.push_back(0.0f); // Internet time counter
}

void CurveNode::evaluate()
{
    // ECurveNode node;

    // node.SetValue(0, getInputValue(0));
    // node.SetValue(1, getInputValue(1));
    // node.SetValue(2, getInputValue(2));
    // node.SetValue(3, getValue(0));
    // node.SetValue(4, getValue(1));

    // node.Evaluate(ImGui::GetIO().DeltaTime);
    // m_values[1] = node.m_timerCounter.value;
    // m_outputs[0] = node.m_out;
}
