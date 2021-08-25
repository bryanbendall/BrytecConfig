#include "Pin.h"
#include <iostream>

const char* Pin::typeNames[] = {
	"12V High Current PWM Output",
	"12V High Current Output",
	"12V Low Current Output",
	"Ground Output",
	"12V Switched Input",
	"Ground Input",
	"5V Switched Input",
	"5V Variable Input",
	"Can Input",
};

const char* Pin::currentNames[] = {
	"5 Amps",
	"7 Amps",
	"9 Amps",
	"11 Amps",
	"13 Amps",
	"15 Amps",
	"17 Amps"
};

Pin::Pin(PinDirection direction, const std::vector<PinTypes> availableTypes) 
	: m_direction(direction), m_availableTypes(availableTypes)
{
	switch(m_direction) {
		case PinDirection::Input:
			addNode(NodeTypes::Raw_Input_Value, {50.0f, 50.0f});
			addNode(NodeTypes::Final_Input_Value, {250.0f, 50.0f});
			m_nodes[1]->getInput(0) = {m_nodes[0], 0};
			break;
		case PinDirection::Output:
			addNode(NodeTypes::Output, {250.0f, 50.0f});
			break;
	}

}

Pin::~Pin()
{
}

std::shared_ptr<Node>& Pin::getNode(int id) {
	for(auto& n : m_nodes) {
		if(n->getId() == id)
			return n;
	}
}

void Pin::addNode(NodeTypes type) 
{
	addNode(type, {0.0f, 0.0f});
}

void Pin::addNode(NodeTypes type, ImVec2 position) 
{
	m_nodes.push_back(std::make_shared<Node>(m_nodeIds, position, type));
	m_nodeIds++;
}

void Pin::sortNodes() 
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

float Pin::getValue(int attributeIndex) {
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

void Pin::deleteNode(int nodeId) 
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

void Pin::evaluateAllNodes() 
{
	for(auto& node : m_nodes)
		node->evaluate();
}

void Pin::traverseConnections(std::shared_ptr<Node> node, std::deque<std::shared_ptr<Node>>& newDeque, std::deque<std::shared_ptr<Node>>& loopCheck)
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
