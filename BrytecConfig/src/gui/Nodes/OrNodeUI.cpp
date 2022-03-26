#include "OrNodeUI.h"

#include "gui/NodeUI.h"

void OrNodeUI::draw(std::shared_ptr<Node> node)
{
    NodeUI::InputBool(node, 0, "Input");
    NodeUI::InputBool(node, 1, "Input");
    NodeUI::InputBool(node, 2, "Input");
    NodeUI::InputBool(node, 3, "Input");
    NodeUI::InputBool(node, 4, "Input");

    NodeUI::Ouput(node, 0, "Result", NodeWindow::boolColor);
}