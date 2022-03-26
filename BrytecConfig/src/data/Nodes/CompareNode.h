#pragma once

#include "data/Node.h"

class CompareNode : public Node {

public:
    CompareNode(int id, ImVec2 position, NodeTypes type);
    void evaluate() override;
};