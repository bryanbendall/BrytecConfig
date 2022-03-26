#pragma once

#include "data/Node.h"

class FinalValueNode : public Node {

public:
    FinalValueNode(int id, ImVec2 position, NodeTypes type);
    void evaluate() override;
};