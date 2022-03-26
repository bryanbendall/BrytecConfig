#pragma once

#include "data/Node.h"

class SwitchNode : public Node {

public:
    SwitchNode(int id, ImVec2 position, NodeTypes type);
    void evaluate() override;
};