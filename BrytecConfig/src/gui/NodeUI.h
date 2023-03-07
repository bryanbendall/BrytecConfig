#pragma once

#include "NodeWindow.h"
#include "data/Node.h"

class NodeUI {

public:
    static void drawNode(std::shared_ptr<Node> node, NodeWindow::Mode& mode, std::weak_ptr<NodeGroup> nodeGroup, float nodeWidth);
    static void drawUnimplimentedNode(std::shared_ptr<Node> node);
};