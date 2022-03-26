#include "CurveNodeUI.h"

#include "gui/NodeUI.h"

const char* CurveNodeUI::s_curveNames[(int)ECurveNode::Types::Count] = {
    "Toggle",
    "Linear",
    "Expontial",
    "Breathing"
};

void CurveNodeUI::draw(std::shared_ptr<Node> node)
{
    NodeUI::InputBool(node, 0, "Input");

    NodeUI::ValueCombo(node, 0, CurveNodeUI::s_curveNames, (int)ECurveNode::Types::Count);

    NodeUI::InputBool(node, 1, "Repeat");

    NodeUI::InputFloat(node, 2, "Time", 0.0f, 10.0f, 0.05f);

    NodeUI::Ouput(node, 0, "Result", NodeWindow::zeroToOneColor);
}