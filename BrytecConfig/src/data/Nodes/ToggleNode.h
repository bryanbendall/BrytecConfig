#pragma once

#include "data/Node.h"

class ToggleNode : public Node {

public:
    ToggleNode(int id, ImVec2 position, NodeTypes type);
    void evaluate() override;
};