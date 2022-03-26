#pragma once

#include "data/Node.h"

class MathNode : public Node {

public:
    MathNode(int id, ImVec2 position, NodeTypes type);
    void evaluate() override;
};