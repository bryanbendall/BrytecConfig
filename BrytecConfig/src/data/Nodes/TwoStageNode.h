#pragma once

#include "data/Node.h"

class TwoStageNode : public Node {

public:
    TwoStageNode(int id, ImVec2 position, NodeTypes type);
    void evaluate() override;
};