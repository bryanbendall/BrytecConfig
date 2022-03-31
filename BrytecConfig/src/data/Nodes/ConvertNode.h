#pragma once

#include "data/Node.h"

class ConvertNode : public Node {

public:
    ConvertNode(int id, ImVec2 position, NodeTypes type);
    void evaluate() override;
};