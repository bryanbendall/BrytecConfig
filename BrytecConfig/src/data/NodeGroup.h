#pragma once

#include "BrytecConfigEmbedded/IOTypes.h"
#include "Node.h"
#include "Selectable.h"
#include "utils/UUID.h"
#include <deque>
#include <memory>
#include <string>

class NodeGroup : public Selectable, public std::enable_shared_from_this<NodeGroup> {

public:
    NodeGroup();
    NodeGroup(UUID uuid);
    NodeGroup(const NodeGroup& other);

    std::string& getName() { return m_name; }
    void setName(const std::string& name) { m_name = name; }

    IOTypes::Types& getType() { return m_type; }
    void setType(IOTypes::Types type) { m_type = type; }

    bool& getEnabled() { return m_enabled; }
    void setEnabled(bool state) { m_enabled = state; }

    uint8_t& getCurrentLimit() { return m_currentLimit; }
    void setCurrentLimit(uint8_t currentLimit) { m_currentLimit = currentLimit; }

    bool& getAlwaysRetry() { return m_alwaysRetry; }
    void setAlwaysRetry(bool alwaysRetry) { m_alwaysRetry = alwaysRetry; }

    uint8_t& getMaxRetries() { return m_maxRetries; }
    void setMaxRetries(uint8_t maxRetries) { m_maxRetries = maxRetries; }

    float& getRetryDelay() { return m_retryDelay; }
    void setRetryDelay(float retryDealy) { m_retryDelay = retryDealy; }

    const UUID& getId() { return m_uuid; }
    void setId(const UUID& uuid) { m_uuid = uuid; }

    float getValue(int attributeIndex);

    bool getAssigned() { return m_assignedToPin; }
    void setAssigned(bool state) { m_assignedToPin = state; }

    std::deque<std::shared_ptr<Node>>& getNodes() { return m_nodes; }
    std::shared_ptr<Node> getNode(int id);
    int getNodeIndex(std::shared_ptr<Node> node);
    std::shared_ptr<Node> addNode(NodeTypes type, ImVec2 position = { 0.0f, 0.0f });
    void deleteNode(int nodeId);
    void sortNodes();
    void evaluateAllNodes();
    bool hasNodeType(NodeTypes type);

private:
    void traverseConnections(std::shared_ptr<Node> node, std::deque<std::shared_ptr<Node>>& newDeque, std::deque<std::shared_ptr<Node>>& loopCheck);

private:
    std::string m_name = "Unnamed";
    UUID m_uuid = 0;
    IOTypes::Types m_type = IOTypes::Types::Undefined;
    bool m_enabled = true;
    uint8_t m_currentLimit = 0;
    bool m_alwaysRetry = false;
    uint8_t m_maxRetries = 0;
    float m_retryDelay = 1.0f;
    bool m_assignedToPin = false;
    std::deque<std::shared_ptr<Node>> m_nodes;
    int m_nodesIds = 0;
};