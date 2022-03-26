#include "TwoStageNodeUI.h"

#include "gui/NodeUI.h"

void TwoStageNodeUI::draw(std::shared_ptr<Node> node)
{
    NodeUI::InputBool(node, 0, "Stage 1");

    NodeUI::InputFloat(node, 1, "Stage 1");

    NodeUI::InputBool(node, 2, "Stage 2");

    NodeUI::InputFloat(node, 3, "Stage 2");

    NodeUI::Ouput(node, 0, "Result");
}