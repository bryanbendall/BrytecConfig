#include "MathNodeUI.h"

#include "gui/NodeUI.h"

const char* MathNodeUI::s_mathNames[(int)EMathNode::Types::Count] = {
    "Add",
    "Subtract",
    "Multiply",
    "Divide"
};

void MathNodeUI::draw(std::shared_ptr<Node> node)
{
    NodeUI::InputFloat(node, 0, "Value");

    NodeUI::InputFloat(node, 1, "Value");

    NodeUI::ValueCombo(node, 0, MathNodeUI::s_mathNames, (int)EMathNode::Types::Count);

    NodeUI::Ouput(node, 0, "Result");
}