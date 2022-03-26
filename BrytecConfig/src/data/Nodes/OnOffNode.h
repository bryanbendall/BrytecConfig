#pragma once

#include "data/Node.h"

class OnOffNode : public Node {

public:
    OnOffNode(int id, ImVec2 position, NodeTypes type);
    void evaluate() override;
};