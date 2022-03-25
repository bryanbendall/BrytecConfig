#pragma once

#include "data/Node.h"

class AndNode : public Node {

public:
    AndNode(int id, ImVec2 position, NodeTypes type);
    void evaluate() override;
};