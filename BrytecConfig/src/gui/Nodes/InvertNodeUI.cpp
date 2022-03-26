#include "InvertNodeUI.h"

#include "gui/NodeUI.h"

void InvertNodeUI::draw(std::shared_ptr<Node> node)
{
    NodeUI::InputBool(node, 0, "Input");

    NodeUI::Ouput(node, 0, "Output", NodeWindow::boolColor);
}