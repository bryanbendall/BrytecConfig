#pragma once

#include "data/Node.h"

class OrNode : public Node {

public:
    OrNode(int id, ImVec2 position, NodeTypes type);
    void evaluate() override;
};