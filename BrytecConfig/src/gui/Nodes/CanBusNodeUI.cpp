#include "CanBusNodeUI.h"

#include "gui/NodeUI.h"

void CanBusNodeUI::draw(std::shared_ptr<Node> node)
{
    NodeUI::Ouput(node, 0, "Id");
    NodeUI::Ouput(node, 1, "Data 0");
    NodeUI::Ouput(node, 2, "Data 1");
    NodeUI::Ouput(node, 3, "Data 2");
    NodeUI::Ouput(node, 4, "Data 3");
    NodeUI::Ouput(node, 5, "Data 4");
    NodeUI::Ouput(node, 6, "Data 5");
    NodeUI::Ouput(node, 7, "Data 6");
    NodeUI::Ouput(node, 8, "Data 7");
}