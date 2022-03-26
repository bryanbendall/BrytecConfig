#pragma once

#include "data/Node.h"

class InvertNode : public Node {

public:
    InvertNode(int id, ImVec2 position, NodeTypes type);
    void evaluate() override;
};