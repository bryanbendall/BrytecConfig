#pragma once

#include "data/Node.h"
#include "gui/NodeWindow.h"

class NodeGroupNodeUI {

public:
    static void draw(std::shared_ptr<Node> node, NodeWindow::Mode& mode, std::weak_ptr<NodeGroup> nodeGroup);
};