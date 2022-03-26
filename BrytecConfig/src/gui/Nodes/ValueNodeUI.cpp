#include "ValueNodeUI.h"

#include "gui/NodeUI.h"

void ValueNodeUI::draw(std::shared_ptr<Node> node)
{
    NodeUI::ValueFloat(node, 0, "Value");

    NodeUI::Ouput(node, 0, "Output");
}