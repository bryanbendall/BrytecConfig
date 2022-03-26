#include "PushButtonNodeUI.h"

#include "gui/NodeUI.h"

void PushButtonNodeUI::draw(std::shared_ptr<Node> node)
{
    NodeUI::InputBool(node, 0, "Button");

    NodeUI::InputBool(node, 1, "Neutral Safety");

    NodeUI::InputBool(node, 2, "Enging Running");

    NodeUI::Ouput(node, 0, "Ignition", NodeWindow::boolColor);

    NodeUI::Ouput(node, 1, "Starter", NodeWindow::boolColor);
}