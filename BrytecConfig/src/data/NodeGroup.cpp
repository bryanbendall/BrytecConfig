#include "NodeGroup.h"

NodeGroup::NodeGroup()
    : m_uuid(UUID())
{
    auto initalNode = addNode(Embedded::NodeTypes::Initial_Value, { 50.0f, 50.0f });
    auto finalNode = addNode(Embedded::NodeTypes::Final_Value, { 350.0f, 50.0f });
    finalNode->setInput(0, { initalNode, 0, 0.0f });
}

NodeGroup::NodeGroup(UUID uuid)
    : m_uuid(uuid)
{
}

NodeGroup::NodeGroup(const NodeGroup& other)
    : m_uuid(UUID())
{
    m_name = other.m_name;
    m_type = other.m_type;
    m_enabled = other.m_enabled;

    // Copy nodes
    for (auto oldNode : other.m_nodes) {
        addNode(oldNode->getType(), oldNode->getPosition());
    }

    // Copy node data
    for (size_t nodeIndex = 0; nodeIndex < m_nodes.size(); nodeIndex++) {
        auto& newNode = m_nodes[nodeIndex];
        auto& oldNode = other.m_nodes[nodeIndex];

        // Copy node connections
        for (size_t inputIndex = 0; inputIndex < oldNode->getInputs().size(); inputIndex++) {
            if (auto oldConnectedNode = oldNode->getInputs()[inputIndex].ConnectedNode.lock()) {
                for (size_t oldConnectedNodeIndex = 0; oldConnectedNodeIndex < m_nodes.size(); oldConnectedNodeIndex++) {
                    if (other.m_nodes[oldConnectedNodeIndex] == oldConnectedNode)
                        newNode->setInput(inputIndex, { m_nodes[oldConnectedNodeIndex], oldNode->getInputs()[inputIndex].OutputIndex, oldNode->getInputs()[inputIndex].DefaultValue });
                }
            }
        }

        // Copy node values
        for (size_t valueIndex = 0; valueIndex < oldNode->getValues().size(); valueIndex++)
            newNode->getValue(valueIndex) = oldNode->getValue(valueIndex);
    }
}

std::shared_ptr<Node> NodeGroup::getNode(int id)
{
    for (auto& n : m_nodes) {
        if (n->getId() == id)
            return n;
    }

    return nullptr;
}

int NodeGroup::getNodeIndex(std::shared_ptr<Node> node)
{
    for (size_t i = 0; i < m_nodes.size(); i++) {
        if (m_nodes[i] == node)
            return i;
    }

    return -1;
}

std::shared_ptr<Node> NodeGroup::addNode(Embedded::NodeTypes type, ImVec2 position)
{
    std::shared_ptr<Node> node = std::make_shared<Node>(m_nodesIds, position, type);
    m_nodes.push_back(node);
    m_nodesIds++;
    return node;
}

void NodeGroup::sortNodes()
{
    for (auto& n : m_nodes)
        n->setLoopFound(false);

    std::deque<std::shared_ptr<Node>> newDeque;
    std::deque<std::shared_ptr<Node>> loopCheck;

    std::shared_ptr<Node> lastNode;
    for (auto& n : m_nodes) {
        if (n->getType() == Embedded::NodeTypes::Final_Value) {
            lastNode = n;
            break;
        }
    }
    if (!lastNode)
        return;

    traverseConnections(lastNode, newDeque, loopCheck);

    //add remaining nodes at begining
    for (auto n : m_nodes) {
        newDeque.push_front(n);
    }

    m_nodes = newDeque;
}

float NodeGroup::getValue(int attributeIndex)
{
    uint8_t nodeIndex = attributeIndex >> 8;
    uint8_t ioIndex = attributeIndex;

    auto node = getNode(nodeIndex);
    if (!node)
        return 0.0f;

    if (ioIndex >= node->getOutputs().size()) {
        int inputIndex = ioIndex - node->getOutputs().size();
        if (node->getInput(inputIndex).ConnectedNode.expired())
            return 0.0f;
        return node->getInput(inputIndex).ConnectedNode.lock()->getOutputValue(node->getInput(inputIndex).OutputIndex);
    } else {
        return node->getOutputValue(ioIndex);
    }
}

void NodeGroup::deleteNode(int nodeId)
{
    for (size_t i = 0; i < m_nodes.size(); i++) {
        if (m_nodes[i]->getId() == nodeId) {
            if (m_nodes[i]->getType() == Embedded::NodeTypes::Final_Value)
                return;
            m_nodes.erase(m_nodes.begin() + i);
        }
    }
}

void NodeGroup::evaluateAllNodes()
{
    for (auto& node : m_nodes)
        node->evaluate();
}

void NodeGroup::traverseConnections(std::shared_ptr<Node> node, std::deque<std::shared_ptr<Node>>& newDeque, std::deque<std::shared_ptr<Node>>& loopCheck)
{
    if (std::find(m_nodes.begin(), m_nodes.end(), node) == m_nodes.end())
        return;

    if (std::find(loopCheck.begin(), loopCheck.end(), node) != loopCheck.end()) {
        node->setLoopFound(true);
        return;
    }
    loopCheck.push_back(node);

    for (auto& c : node->getInputs()) {
        if (!c.ConnectedNode.expired())
            traverseConnections(c.ConnectedNode.lock(), newDeque, loopCheck);
    }

    auto it = std::find(m_nodes.begin(), m_nodes.end(), node);
    if (it != m_nodes.end()) {
        m_nodes.erase(it);
        newDeque.push_back(node);
    }

    loopCheck.pop_back();
}
