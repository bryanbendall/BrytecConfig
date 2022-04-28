#include "CompareNodeUI.h"

#include "gui/NodeUI.h"

const char* CompareNodeUI::s_compareNames[(int)CompareType::Count] = {
    "Equal",
    "Not Equal",
    "Greater Than",
    "Greater Equal To",
    "Less Than",
    "Less Equal To"
};

void CompareNodeUI::draw(std::shared_ptr<Node> node)
{
    NodeUI::InputFloat(node, 0, "Value 1");

    NodeUI::InputFloat(node, 1, "Value 2");

    NodeUI::ValueCombo(node, 0, CompareNodeUI::s_compareNames, (int)CompareType::Count);

    NodeUI::Ouput(node, 0, "Result", NodeWindow::boolColor);
}