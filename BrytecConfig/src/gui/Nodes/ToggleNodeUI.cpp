#include "ToggleNodeUI.h"

#include "gui/NodeUI.h"

void ToggleNodeUI::draw(std::shared_ptr<Node> node)
{
    NodeUI::InputBool(node, 0, "Input");

    NodeUI::Ouput(node, 0, "Output", NodeWindow::boolColor);
}