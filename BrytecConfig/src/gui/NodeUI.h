#pragma once

#include "NodeWindow.h"
#include "data/Node.h"

class NodeUI {

public:
    static void drawNode(std::shared_ptr<Node> node);
    static void drawUnimplimentedNode(std::shared_ptr<Node> node);

    // UI
    static void SameHeightText(std::string text);
    static void InputFloat(std::shared_ptr<Node>& node, int attribute, std::string label, float min = 0.0f, float max = 0.0f, float speed = 1.0f);
    static void InputBool(std::shared_ptr<Node>& node, int attribute, std::string label);
    static void ValueFloat(std::shared_ptr<Node>& node, int attribute, std::string label, float min = 0.0f, float max = 0.0f, float speed = 1.0f);
    static void ValueCombo(std::shared_ptr<Node>& node, int attribute, const char* const items[], int items_count);
    static void Ouput(std::shared_ptr<Node>& node, int attribute, std::string label, unsigned int color = NodeWindow::anyValueColor);
    static void OnOffButton(std::shared_ptr<Node>& node, float& value, bool interact);
};