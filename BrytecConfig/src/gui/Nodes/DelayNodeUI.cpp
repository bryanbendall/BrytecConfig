#include "DelayNodeUI.h"

#include "gui/NodeUI.h"

void DelayNodeUI::draw(std::shared_ptr<Node> node)
{
    NodeUI::InputFloat(node, 0, "Input");

    NodeUI::InputFloat(node, 1, "Time");

    NodeUI::Ouput(node, 0, "Output");
}