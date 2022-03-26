#pragma once

#include "data/Node.h"

class CurveNode : public Node {

public:
    CurveNode(int id, ImVec2 position, NodeTypes type);
    void evaluate() override;
};