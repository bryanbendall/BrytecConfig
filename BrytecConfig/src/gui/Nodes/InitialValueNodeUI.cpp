#include "InitialValueNodeUI.h"

#include "gui/NodeUI.h"

void InitialValueNodeUI::draw(std::shared_ptr<Node> node, NodeWindow::Mode& mode)
{
    if (mode == NodeWindow::Mode::Simulation) {

        NodeUI::ValueFloat(node, 0, "Value", 0.0f, 5.0f, 0.01f);

        NodeUI::OnOffButton(node, node->getValue(0), true);
    }

    NodeUI::Ouput(node, 0, "Value");
}