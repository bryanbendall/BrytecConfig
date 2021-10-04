#pragma once

#include "Node.h"
#include <deque>
#include <memory>
#include <string>
#include "Selectable.h"
#include "IOTypes.h"

class NodeGroup : public Selectable {

	std::deque<std::shared_ptr<Node>> m_nodes;
	IOTypes::Types m_type = IOTypes::Types::Undefined;
	std::string m_name = "Unnamed";
	bool m_enabled = true;
	bool m_assignedToPin = false;
	int m_nodeIds = 0;

public:
	NodeGroup();
	std::deque<std::shared_ptr<Node>>& getNodes() { return m_nodes; }
	std::shared_ptr<Node> getNode(int id);
	std::string& getName() { return m_name; }
	bool& getEnabled() { return m_enabled; }
	IOTypes::Types& getType() { return m_type; }
	void addNode(NodeTypes type, ImVec2 position = {0.0f, 0.0f});
	void sortNodes();
	float getValue(int attributeIndex);
	void deleteNode(int nodeId);
	void evaluateAllNodes();
	bool getAssigned() { return m_assignedToPin; }
	void setAssigned(bool state) { m_assignedToPin = state; }

private:
	void traverseConnections(std::shared_ptr<Node> node, std::deque<std::shared_ptr<Node>>& newDeque, std::deque<std::shared_ptr<Node>>& loopCheck);

};