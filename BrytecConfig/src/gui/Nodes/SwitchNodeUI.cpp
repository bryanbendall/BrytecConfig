#include "SwitchNodeUI.h"

#include "gui/NodeUI.h"

void SwitchNodeUI::draw(std::shared_ptr<Node> node)
{
    NodeUI::InputBool(node, 0, "Selection");

    NodeUI::InputFloat(node, 1, "If True");

    NodeUI::InputFloat(node, 2, "If False");

    NodeUI::Ouput(node, 0, "Output");
}