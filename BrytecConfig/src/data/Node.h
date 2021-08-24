#pragma once

#include <imgui.h>
#include <vector>
#include <string>
#include <memory>

class Pin;

enum class NodeTypes {
	Output,
	Final_Input_Value,
	Raw_Input_Value,
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
	unsigned int outputIndex;
};

class Node
{

	int m_id;
	ImVec2 m_position;
	std::string m_name;
	std::vector<float> m_outputs;
	std::vector<NodeConnection> m_inputs;
	std::vector<float> m_values;
	bool m_loopFound = false;
	NodeTypes m_type;
	std::weak_ptr<Pin> m_pinSelecion;

public:
	static const char* s_nodeName[(int) NodeTypes::Count];
	static const char* s_compareNames[(int) CompareTypes::Count];
	static const char* s_mathNames[(int) MathTypes::Count];
	static const char* s_curveNames[(int) CurveTypes::Count];

public:
	Node(int id, ImVec2 position, NodeTypes type);
	void evaluate();

	float getOutputValue(size_t outputIndex);
	int getId() { return m_id; }
	unsigned int getSize();
	ImVec2& getPosition() { return m_position; }
	bool& getLoopFound() { return m_loopFound; }
	std::string& getName() { return m_name; }
	NodeTypes& getType() { return m_type; }
	NodeConnection& getInput(int index) { return m_inputs[index]; }
	std::vector<NodeConnection>& getInputs() { return m_inputs; }
	float& getOutput(int index) { return m_outputs[index]; }
	std::vector<float>& getOutputs() { return m_outputs; }
	float& getValue(int index) { return m_values[index]; }
	std::weak_ptr<Pin>& getPinSelection() { return m_pinSelecion; }

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

};

