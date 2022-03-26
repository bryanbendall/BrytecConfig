#pragma once

#include "BrytecConfigEmbedded/Nodes/ECurveNode.h"
#include "data/Node.h"

class CurveNodeUI {

public:
    static void draw(std::shared_ptr<Node> node);

    static const char* s_curveNames[(int)ECurveNode::Types::Count];
};