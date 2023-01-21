#include "NodeGroup.h"

#include "BrytecConfigEmbedded/EBrytecApp.h"
#include "gui/NotificationWindow.h"
#include "utils/ModuleSerializer.h"

NodeGroup::NodeGroup()
    : m_uuid(UUID())
{
    auto initalNode = addNode(NodeTypes::Initial_Value, { 50.0f, 50.0f });
    auto finalNode = addNode(NodeTypes::Final_Value, { 350.0f, 50.0f });
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
    m_currentLimit = other.m_currentLimit;
    m_alwaysRetry = other.m_alwaysRetry;
    m_maxRetries = other.m_maxRetries;
    m_retryDelay = other.m_retryDelay;

    // Copy nodes
    for (auto oldNode : other.m_nodes) {
        addNode(oldNode->getType(), oldNode->getPosition());
    }

    // Copy node data
    for (size_t nodeIndex = 0; nodeIndex < m_nodes.size(); nodeIndex++) {
        auto& newNode = m_nodes[nodeIndex];
        auto& oldNode = other.m_nodes[nodeIndex];

        // Name
        newNode->setName(oldNode->getName());

        // Copy node connections
        for (size_t inputIndex = 0; inputIndex < oldNode->getInputs().size(); inputIndex++) {
            if (auto oldConnectedNode = oldNode->getInputs()[inputIndex].ConnectedNode.lock()) {
                for (size_t oldConnectedNodeIndex = 0; oldConnectedNodeIndex < m_nodes.size(); oldConnectedNodeIndex++) {
                    if (other.m_nodes[oldConnectedNodeIndex] == oldConnectedNode)
                        newNode->setInput(inputIndex, { m_nodes[oldConnectedNodeIndex], oldNode->getInputs()[inputIndex].OutputIndex, oldNode->getInputs()[inputIndex].DefaultValue });
                }
            } else
                newNode->setInput(inputIndex, { std::weak_ptr<Node>(), -1, oldNode->getInputs()[inputIndex].DefaultValue });
        }

        // Copy node values
        newNode->getValues() = oldNode->getValues();

        // Copy node outputs
        newNode->getOutputs() = oldNode->getOutputs();

        // Node group node
        newNode->setSelectedNodeGroup(oldNode->getSelectedNodeGroup());
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

std::shared_ptr<Node> NodeGroup::addNode(NodeTypes type, ImVec2 position)
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
        if (n->getType() == NodeTypes::Final_Value) {
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
            if (m_nodes[i]->getType() == NodeTypes::Final_Value)
                return;
            m_nodes.erase(m_nodes.begin() + i);
        }
    }
}

void NodeGroup::evaluateAllNodes()
{
    // Serialize node group
    std::shared_ptr<NodeGroup> thisNodeGroup = shared_from_this();

    std::shared_ptr<Module> module = std::make_shared<Module>();
    module->addPhysicalPin();
    module->getPhysicalPins()[0]->setNodeGroup(thisNodeGroup, true);

    ModuleSerializer moduleSer(module);
    BinarySerializer ser = moduleSer.serializeBinary();
    BinaryDeserializer des;
    des.setData(ser.getData().data(), ser.getData().size());

    EBrytecApp::deserializeModule(des);
    EBrytecApp::evaulateJustNodes(ImGui::GetIO().DeltaTime);

    // Extract output values
    for (int index = 0; index < m_nodes.size(); index++) {
        auto node = m_nodes[index];
        auto eNode = EBrytecApp::getNode(index);

        // Outputs
        {
            for (int i = 0; i < node->getOutputs().size(); i++) {
                node->getOutput(i) = *(eNode->GetOutput(i));
            }
        }

        // Values
        {
            for (int i = 0; i < node->getValues().size(); i++) {
                node->getValue(i) = eNode->GetValue(i + node->getInputs().size());
            }
        }

        if (node->getType() == NodeTypes::Node_Group) {
            // TODO
        }
    }
}

bool NodeGroup::hasNodeType(NodeTypes type)
{
    for (auto& node : m_nodes) {
        if (node->getType() == type)
            return true;
    }

    return false;
}

void NodeGroup::traverseConnections(std::shared_ptr<Node> node, std::deque<std::shared_ptr<Node>>& newDeque, std::deque<std::shared_ptr<Node>>& loopCheck)
{
    if (std::find(m_nodes.begin(), m_nodes.end(), node) == m_nodes.end())
        return;

    if (std::find(loopCheck.begin(), loopCheck.end(), node) != loopCheck.end()) {
        node->setLoopFound(true);
        NotificationWindow::add({ "Loop Found!", NotificationType::Error });
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
