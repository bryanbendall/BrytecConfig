#pragma once

#include "data/Node.h"

class InitialValueNode : public Node {

public:
    InitialValueNode(int id, ImVec2 position, NodeTypes type);
    void evaluate() override;
};