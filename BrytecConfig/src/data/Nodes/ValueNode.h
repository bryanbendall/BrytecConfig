#pragma once

#include "data/Node.h"

class ValueNode : public Node {

public:
    ValueNode(int id, ImVec2 position, NodeTypes type);
    void evaluate() override;
};