#pragma once

#include "data/Node.h"

class CanBusNode : public Node {

public:
    CanBusNode(int id, ImVec2 position, NodeTypes type);
    void evaluate() override;
};