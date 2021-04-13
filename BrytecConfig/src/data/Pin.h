#pragma once

#include <deque>
#include "Node.h"
#include "Selectable.h"
#include <memory>
#include <string>

enum class PinTypes {
	Output_12V_Pwm,
	Output_12V,
	Output_12V_Low_Current,
	Output_Ground,
	Input_12V,
	Input_Ground,
	Input_5V,
	Input_5V_Variable,
	Input_Can,
	Count
};

enum class PinCurrentLimit {
	V5,
	V7,
	V9,
	V11,
	V13,
	V15,
	V17,
	Count
};

enum class PinDirection {
	Input,
	Output
};

#define ALL_INPUT_TYPES std::vector<PinTypes>{ PinTypes::Input_12V, PinTypes::Input_Ground, PinTypes::Input_5V, PinTypes::Input_5V_Variable, PinTypes::Input_Can }
#define ALL_OUTPUT_TYPES std::vector<PinTypes>{	PinTypes::Output_12V_Pwm, PinTypes::Output_12V, PinTypes::Output_12V_Low_Current, PinTypes::Output_Ground }

class Pin : public Selectable
{
	std::deque<std::shared_ptr<Node>> m_nodes;
	std::string m_name = "Pin";
	std::string m_pinoutName = "A1";
	PinTypes m_pinType = (PinTypes)-1;
	PinCurrentLimit m_currentLimit = (PinCurrentLimit)-1;
	bool m_enabled = false;
	PinDirection m_direction;
	int m_nodeIds = 0;
	const std::vector<PinTypes> m_availableTypes;

	static const char* typeNames[(size_t)PinTypes::Count];
	static const char* currentNames[(size_t) PinCurrentLimit::Count];

public:
	Pin(PinDirection direction, const std::vector<PinTypes> availableTypes);
	~Pin();
	std::deque<std::shared_ptr<Node>>& getNodes() { return m_nodes; }
	std::shared_ptr<Node>& getNode(int id);
	void addNode(NodeTypes type);
	void addNode(NodeTypes type, ImVec2 position);
	void sortNodes();
	float getValue(int attributeIndex);
	void deleteNode(int nodeId);
	void evaluateAllNodes();

private:
	void traverseConnections(std::shared_ptr<Node> node, std::deque<std::shared_ptr<Node>>& newDeque, std::deque<std::shared_ptr<Node>>& loopCheck);

	friend class PropertiesWindow;
	friend class ModuleWindow;
	friend class NodeWindow;
};

