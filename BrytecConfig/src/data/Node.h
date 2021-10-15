#pragma once

#include <imgui.h>
#include <vector>
#include <string>
#include <memory>

enum class NodeTypes {
	Initial_Value,
	Final_Value,
	Pin,
	And,
	Or,
	Two_Stage,
	Curve,
	Compare,
	On_Off,
	Invert,
	Toggle,
	Delay,
	Push_Button,
	Map_Value,
	Math,
	Value,
	Count
};

enum class CompareTypes {
	Greater,
	GreaterEqual,
	Less,
	LessEqual,
	Count
};

enum class MathTypes {
	Add,
	Subtract,
	Multiply,
	Divide,
	Count
};

enum class CurveTypes {
	Toggle,
	Linear,
	Exponential,
	Breathing,
	Count
};

class Node;
struct NodeConnection {
	std::weak_ptr<Node> node;
	int outputIndex;
};

class Node
{

public:
	Node(int id, ImVec2 position, NodeTypes type);

	void evaluate();

	std::string& getName() { return m_name; }
	ImVec2& getPosition() { return m_position; }
	
	NodeTypes& getType() { return m_type; }

	int getId() { return m_id; }
	int getOutputId(int index) { return (m_id << 8) + index; }
	int getIntputId(int index) { return (m_id << 8) + m_outputs.size() + index; }
	int getValueId(int index) { return (m_id << 8) + m_values.size() + m_outputs.size() + index; }
	
	float getOutputValue(size_t outputIndex);
	std::vector<float>& getValues() { return m_values; }
	float& getValue(int index) { return m_values[index]; }
	void setValue(int index, float value) { m_values[index] = value; }

	NodeConnection& getInput(int index) { return m_inputs[index]; }
	std::vector<NodeConnection>& getInputs() { return m_inputs; }
	void setInput(int inputIndex, NodeConnection nodeConnection);

	float& getOutput(int index) { return m_outputs[index]; }
	std::vector<float>& getOutputs() { return m_outputs; }

	const bool& getLoopFound() { return m_loopFound; }
	void setLoopFound(bool state) { m_loopFound = state; }

	unsigned int getBytesSize();

private:
	void evaluateAnd();
	void evaluateOr();
	void evaluateInvert();
	void evaluateTwoStage();
	void evaluateOnOff();
	void evaluateToggle();
	void evaluateDelay();
	void evaluateCompare();
	void evaluateMath();
	void evaluateMap();
	void evaulateCurve();
	void evaluatePushButton();

	float& getInputValue(int inputIndex);
	bool hasConnection(int inputIndex);

public:
	static const char* s_nodeName[(int) NodeTypes::Count];
	static const char* getTypeName(NodeTypes type) { return s_nodeName[(int)type]; }
	static const char* s_compareNames[(int) CompareTypes::Count];
	static const char* s_mathNames[(int) MathTypes::Count];
	static const char* s_curveNames[(int) CurveTypes::Count];

private:
	std::string m_name;
	NodeTypes m_type;
	int m_id;
	ImVec2 m_position;
	std::vector<float> m_outputs;
	std::vector<NodeConnection> m_inputs;
	std::vector<float> m_values;
	bool m_loopFound = false;

};

