#pragma once

#include "Node.h"
#include <deque>
#include <memory>
#include <string>
#include "Selectable.h"
#include "IOTypes.h"

class NodeGroup : public Selectable 
{

public:
	NodeGroup();
	
	std::string& getName() { return m_name; }
	void setName(const std::string& name) { m_name = name; }

	IOTypes::Types& getType() { return m_type; }
	void setType(IOTypes::Types type) { m_type = type; }

	bool& getEnabled() { return m_enabled; }
	void setEnabled(bool state) { m_enabled = state; }

	float getValue(int attributeIndex);

	bool getAssigned() { return m_assignedToPin; }
	void setAssigned(bool state) { m_assignedToPin = state; }
	
	std::deque<std::shared_ptr<Node>>& getNodes() { return m_nodes; }
	std::shared_ptr<Node> getNode(int id);
	void addNode(NodeTypes type, ImVec2 position = {0.0f, 0.0f});
	void deleteNode(int nodeId);
	void sortNodes();
	void evaluateAllNodes();

private:
	void traverseConnections(std::shared_ptr<Node> node, std::deque<std::shared_ptr<Node>>& newDeque, std::deque<std::shared_ptr<Node>>& loopCheck);

private:
	std::string m_name = "Unnamed";
	IOTypes::Types m_type = IOTypes::Types::Undefined;
	bool m_enabled = true;
	bool m_assignedToPin = false;
	std::deque<std::shared_ptr<Node>> m_nodes;
	int m_nodeIds = 0;

};