#pragma once

#include "BrytecConfigEmbedded/Nodes/EMathNode.h"
#include "data/Node.h"

class MathNodeUI {

public:
    static void draw(std::shared_ptr<Node> node);

    static const char* s_mathNames[(int)EMathNode::Types::Count];
};