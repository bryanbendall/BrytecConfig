#pragma once

#include "data/Node.h"

class DelayNode : public Node {

public:
    DelayNode(int id, ImVec2 position, NodeTypes type);
    void evaluate() override;
};