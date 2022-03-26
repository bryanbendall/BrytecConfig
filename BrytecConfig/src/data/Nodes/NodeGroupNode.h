#pragma once

#include "data/Node.h"

class NodeGroupNode : public Node {

public:
    NodeGroupNode(int id, ImVec2 position, NodeTypes type);
    void evaluate() override;
};