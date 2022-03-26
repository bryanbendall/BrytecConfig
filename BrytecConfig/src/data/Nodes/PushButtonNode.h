#pragma once

#include "data/Node.h"

class PushButtonNode : public Node {

public:
    PushButtonNode(int id, ImVec2 position, NodeTypes type);
    void evaluate() override;
};