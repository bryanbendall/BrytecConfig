#pragma once

#include "data/Node.h"

class MapValueNode : public Node {

public:
    MapValueNode(int id, ImVec2 position, NodeTypes type);
    void evaluate() override;
};