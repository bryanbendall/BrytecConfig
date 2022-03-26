#pragma once

#include "data/Node.h"
#include "gui/NodeWindow.h"

class InitialValueNodeUI {

public:
    static void draw(std::shared_ptr<Node> node, NodeWindow::Mode& mode);
};