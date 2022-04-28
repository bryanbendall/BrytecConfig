#pragma once

#include "BrytecConfigEmbedded/Nodes/ECompareNode.h"
#include "data/Node.h"

class CompareNodeUI {

public:
    static void draw(std::shared_ptr<Node> node);

    static const char* s_compareNames[(int)CompareType::Count];
};