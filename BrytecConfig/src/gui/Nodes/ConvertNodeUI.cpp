#include "ConvertNodeUI.h"

#include "gui/NodeUI.h"

#include "BrytecConfigEmbedded/Nodes/EMathNode.h"

void ConvertNodeUI::draw(std::shared_ptr<Node> node)
{
    NodeUI::InputFloat(node, 0, "Data 0");
    if (node->getValue(1) >= 1.0f)
        NodeUI::InputFloat(node, 1, "Data 1");
    if (node->getValue(1) >= 2.0f) {
        NodeUI::InputFloat(node, 2, "Data 2");
        NodeUI::InputFloat(node, 3, "Data 3");
    }

    const char* endians[2] = { "Big", "Little" };
    NodeUI::ValueCombo(node, 0, endians, 2);

    const char* outputs[3] = { "uint8", "uint16", "uint32" };
    NodeUI::ValueCombo(node, 1, outputs, 3);

    NodeUI::Ouput(node, 0, "Output");
}