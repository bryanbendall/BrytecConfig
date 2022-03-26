#include "OnOffNodeUI.h"

#include "gui/NodeUI.h"

void OnOffNodeUI::draw(std::shared_ptr<Node> node)
{
    NodeUI::InputBool(node, 0, "On");

    NodeUI::InputBool(node, 1, "Off");

    NodeUI::Ouput(node, 0, "Output", NodeWindow::boolColor);
}