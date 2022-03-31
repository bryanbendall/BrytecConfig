#include "CanBusNodeUI.h"

#include "gui/NodeUI.h"

void CanBusNodeUI::draw(std::shared_ptr<Node> node)
{
    NodeUI::InputFloat(node, 0, "Id", 0, 0.0f, 300.0f);

    NodeUI::Ouput(node, 0, "Data 0");
    NodeUI::Ouput(node, 1, "Data 1");
    NodeUI::Ouput(node, 2, "Data 2");
    NodeUI::Ouput(node, 3, "Data 3");
    NodeUI::Ouput(node, 4, "Data 4");
    NodeUI::Ouput(node, 5, "Data 5");
    NodeUI::Ouput(node, 6, "Data 6");
    NodeUI::Ouput(node, 7, "Data 7");
}