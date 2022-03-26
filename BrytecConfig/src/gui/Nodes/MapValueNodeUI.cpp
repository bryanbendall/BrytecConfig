#include "MapValueNodeUI.h"

#include "gui/NodeUI.h"

void MapValueNodeUI::draw(std::shared_ptr<Node> node)
{
    NodeUI::InputFloat(node, 0, "Input");

    NodeUI::InputFloat(node, 1, "From Min");
    NodeUI::InputFloat(node, 2, "From Max");

    NodeUI::InputFloat(node, 3, "To Min");
    NodeUI::InputFloat(node, 4, "To Max");

    NodeUI::Ouput(node, 0, "Result");
}