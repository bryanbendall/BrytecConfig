#include "NodeGroup.h"

NodeGroup::NodeGroup() 
{
	addNode(NodeTypes::Raw_Input_Value, {50.0f, 50.0f});
}

std::shared_ptr<Node> NodeGroup::getNode(int id) 
{
	for(auto& n : m_nodes) {
		if(n->getId() == id)
			return n;
	}
	assert(false);
	return nullptr;
}

void NodeGroup::addNode(NodeTypes type, ImVec2 position) 
{
	m_nodes.push_back(std::make_shared<Node>(m_nodeIds, position, type));
	m_nodeIds++;
}

void NodeGroup::sortNodes() 
{
	for(auto& n : m_nodes)
		n->getLoopFound() = false;

	std::deque<std::shared_ptr<Node>> newDeque;
	std::deque<std::shared_ptr<Node>> loopCheck;

	std::shared_ptr<Node> last;
	for(auto& n : m_nodes) {
		if(n->getType() == NodeTypes::Output) {
			last = n;
			break;
		}
	}
	if(!last)
		return;

	traverseConnections(last, newDeque, loopCheck);

	//add remaining nodes at begining
	for(auto n : m_nodes) {
		newDeque.push_front(n);
	}

	m_nodes = newDeque;
}

float NodeGroup::getValue(int attributeIndex) 
{
	uint8_t nodeIndex = attributeIndex >> 8;
	uint8_t ioIndex = attributeIndex;

	auto& node = getNode(nodeIndex);
	if(ioIndex >= node->getOutputs().size()) {
		int inputIndex = ioIndex - node->getOutputs().size();
		if(node->getInput(inputIndex).node.expired())
			return 0.0f;
		return node->getInput(inputIndex).node.lock()->getOutputValue(node->getInput(inputIndex).outputIndex);
	} else {
		return node->getOutputValue(ioIndex);
	}
}

void NodeGroup::deleteNode(int nodeId) 
{
	for(size_t i = 0; i < m_nodes.size(); i++) {
		if(m_nodes[i]->getId() == nodeId) {
			if(m_nodes[i]->getType() == NodeTypes::Output ||
				m_nodes[i]->getType() == NodeTypes::Raw_Input_Value ||
				m_nodes[i]->getType() == NodeTypes::Final_Input_Value)
				return;
			m_nodes.erase(m_nodes.begin() + i);
		}
	}
}

void NodeGroup::evaluateAllNodes() 
{
	for(auto& node : m_nodes)
		node->evaluate();
}

void NodeGroup::traverseConnections(std::shared_ptr<Node> node, std::deque<std::shared_ptr<Node>>& newDeque, std::deque<std::shared_ptr<Node>>& loopCheck) 
{
	if(std::find(m_nodes.begin(), m_nodes.end(), node) == m_nodes.end())
		return;

	if(std::find(loopCheck.begin(), loopCheck.end(), node) != loopCheck.end()) {
		node->getLoopFound() = true;
		return;
	}
	loopCheck.push_back(node);

	for(auto& c : node->getInputs()) {
		if(!c.node.expired())
			traverseConnections(c.node.lock(), newDeque, loopCheck);
	}

	auto it = std::find(m_nodes.begin(), m_nodes.end(), node);
	if(it != m_nodes.end()) {
		m_nodes.erase(it);
		newDeque.push_back(node);
	}

	loopCheck.pop_back();
}
