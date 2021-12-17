#include "Node.h"
#include <iostream>
#include "NodeGroup.h"

const char* Node::s_nodeName[(int) Embedded::NodeTypes::Count] = {
	"Final Value",
	"Initial Value",
	"Node Group",
	"And",
	"Or",
	"Two Stage",
	"Curve",
	"Compare",
	"On/Off",
	"Invert",
	"Toggle",
	"Delay",
	"Push Button Start",
	"Map Value",
	"Math",
	"Value",
	"Select"
};

const char* Node::s_compareNames[(int) Embedded::CompareNode::Types::Count] = {
	"Greater Than",
	"Greater Equal To",
	"Less Than",
	"Less Equal To"
};

const char* Node::s_mathNames[(int) Embedded::MathNode::Types::Count] = {
	"Add",
	"Subtract",
	"Multiply",
	"Divide"
};

const char* Node::s_curveNames[(int) Embedded::CurveNode::Types::Count] = {
	"Toggle",
	"Linear",
	"Expontial",
	"Breathing"
};

Node::Node(int id, ImVec2 position, Embedded::NodeTypes type)
	: m_id(id), m_position(position), m_type(type), m_name("")
{
	int inputs = 0;
	int outputs = 0;
	int values = 0;

	switch (type)
	{
	case Embedded::NodeTypes::Initial_Value:
		outputs = 1;
		values = 1;
		break;
	case Embedded::NodeTypes::Final_Value:
		inputs = 1;
		break;
	case Embedded::NodeTypes::Node_Group:
		outputs = 1;
		values =  1;
		break;
	case Embedded::NodeTypes::And:
		inputs =  5;
		outputs = 1;
		break;
	case Embedded::NodeTypes::Or:
		inputs =  5;
		outputs = 1;
		break;
	case Embedded::NodeTypes::Two_Stage:
		inputs =  2;
		outputs = 1;
		m_values.push_back(50.0f);
		m_values.push_back(100.0f);
		break;
	case Embedded::NodeTypes::Curve:
		inputs = 1;
		outputs = 1;
		m_values.push_back(0.0f);
		m_values.push_back(0.0f);
		m_values.push_back(1.0f);
		m_values.push_back(0.0f);
		break;
	case Embedded::NodeTypes::Compare:
		inputs = 2;
		outputs = 1;
		values = 1;
		break;
	case Embedded::NodeTypes::On_Off:
		inputs = 2;
		outputs = 1;
		break;
	case Embedded::NodeTypes::Invert:
		inputs = 1;
		outputs = 1;
		break;
	case Embedded::NodeTypes::Toggle:
		inputs = 1;
		outputs = 1;
		values = 1;
		break;
	case Embedded::NodeTypes::Delay:
		inputs = 1;
		outputs = 1;
		m_values.push_back(1.0f);
		m_values.push_back(0.0f);
		break;
	case Embedded::NodeTypes::Push_Button:
		inputs = 3;
		outputs = 2;
		values = 1;
		break;
	case Embedded::NodeTypes::Map_Value:
		inputs =  1;
		outputs = 1;
		m_values.push_back(0.0f);
		m_values.push_back(1.0f);
		m_values.push_back(0.0f);
		m_values.push_back(100.0f);
		break;
	case Embedded::NodeTypes::Math:
		inputs = 2;
		outputs = 1;
		values = 1;
		break;
	case Embedded::NodeTypes::Value:
		outputs = 1;
		values = 1;
		break;
	case Embedded::NodeTypes::Select:
		inputs = 3;
		outputs = 1;
		break;
	case Embedded::NodeTypes::Count:
		assert(false);
		break;
	default:
		assert(false);
		break;
	}

	for(int i = 0; i < inputs; i++)
		m_inputs.push_back(NodeConnection());
	
	for(int j = 0; j < outputs; j++)
		m_outputs.push_back(0.0f);

	for(int k = 0; k < values; k++)
		m_values.push_back(0.0f);

}

float Node::getOutputValue(size_t outputIndex) {
	if(outputIndex >= m_outputs.size())
		return 0.0f;

	return m_outputs[outputIndex];
}

void Node::setInput(int inputIndex, NodeConnection nodeConnection)
{
	assert(!(inputIndex > (int) m_inputs.size()));
	m_inputs[inputIndex] = nodeConnection;
}

unsigned int Node::getBytesSize()
{
	return 0;
}

void Node::evaluate() {
	switch(m_type) {
		case Embedded::NodeTypes::Initial_Value:		m_outputs[0] = m_values[0];	break;
		case Embedded::NodeTypes::Final_Value:										break;
		case Embedded::NodeTypes::Node_Group:			m_outputs[0] = m_values[0]; break;
		case Embedded::NodeTypes::And:					evaluateAnd();				break;
		case Embedded::NodeTypes::Or:					evaluateOr();				break;
		case Embedded::NodeTypes::Two_Stage:			evaluateTwoStage();			break;
		case Embedded::NodeTypes::Curve:				evaulateCurve();			break;
		case Embedded::NodeTypes::Compare:				evaluateCompare();			break;
		case Embedded::NodeTypes::On_Off:				evaluateOnOff();			break;
		case Embedded::NodeTypes::Invert:				evaluateInvert();			break;
		case Embedded::NodeTypes::Toggle:				evaluateToggle();			break;
		case Embedded::NodeTypes::Delay:				evaluateDelay();			break;
		case Embedded::NodeTypes::Push_Button:			evaluatePushButton();		break;
		case Embedded::NodeTypes::Map_Value:			evaluateMap();				break;
		case Embedded::NodeTypes::Math:					evaluateMath();				break;
		case Embedded::NodeTypes::Value:				m_outputs[0] = m_values[0];	break;
		case Embedded::NodeTypes::Select:				evaluateSelect();			break;
		case Embedded::NodeTypes::Count:											break;
		default:										assert(false);				break;
	}
}

void Node::evaluateAnd() 
{
	Embedded::AndNode node = {
		(hasConnection(0) ? &getInputValue(0) : nullptr),
		(hasConnection(1) ? &getInputValue(1) : nullptr),
		(hasConnection(2) ? &getInputValue(2) : nullptr),
		(hasConnection(3) ? &getInputValue(3) : nullptr),
		(hasConnection(4) ? &getInputValue(4) : nullptr)
	};
	node.Evaluate(ImGui::GetIO().DeltaTime);
	m_outputs[0] = node.out;
}

void Node::evaluateOr() 
{
	Embedded::OrNode node = {
		(hasConnection(0) ? &getInputValue(0) : nullptr),
		(hasConnection(1) ? &getInputValue(1) : nullptr),
		(hasConnection(2) ? &getInputValue(2) : nullptr),
		(hasConnection(3) ? &getInputValue(3) : nullptr),
		(hasConnection(4) ? &getInputValue(4) : nullptr)
	};
	node.Evaluate(ImGui::GetIO().DeltaTime);
	m_outputs[0] = node.out;
}

void Node::evaluateInvert() 
{
	Embedded::InvertNode node = {
		(hasConnection(0) ? &getInputValue(0) : nullptr)
	};
	node.Evaluate(ImGui::GetIO().DeltaTime);
	m_outputs[0] = node.out;
}

void Node::evaluateTwoStage() 
{
	Embedded::TwoStageNode node = {
		(hasConnection(0) ? &getInputValue(0) : nullptr),
		(hasConnection(1) ? &getInputValue(1) : nullptr),
		(uint8_t) m_values[0],
		(uint8_t) m_values[1]
	};
	node.Evaluate(ImGui::GetIO().DeltaTime);
	m_outputs[0] = node.out;
}

void Node::evaluateOnOff() 
{
	Embedded::OnOffNode node = {
		(hasConnection(0) ? &getInputValue(0) : nullptr),
		(hasConnection(1) ? &getInputValue(1) : nullptr)
	};
	node.Evaluate(ImGui::GetIO().DeltaTime);
	m_outputs[0] = node.out;
}

void Node::evaluateToggle() 
{
	Embedded::ToggleNode node = {
		(hasConnection(0) ? &getInputValue(0) : nullptr),
		(bool)m_values[0],
		m_outputs[0]
	};
	node.Evaluate(ImGui::GetIO().DeltaTime);
	m_values[0] = node.lastValue;
	m_outputs[0] = node.out;
}

void Node::evaluateDelay() 
{
	Embedded::DelayNode node = {
		(hasConnection(0) ? &getInputValue(0) : nullptr),
		m_values[0],
		m_values[1]
	};
	node.Evaluate(ImGui::GetIO().DeltaTime);
	m_values[1] = node.counter;
	m_outputs[0] = node.out;
}

void Node::evaluateCompare() 
{
	Embedded::CompareNode node = {
		(hasConnection(0) ? &getInputValue(0) : nullptr),
		(hasConnection(1) ? &getInputValue(1) : nullptr),
		(Embedded::CompareNode::Types)m_values[0]
	};
	node.Evaluate(ImGui::GetIO().DeltaTime);
	m_outputs[0] = node.out;
}

void Node::evaluateMath() 
{
	Embedded::MathNode node = {
		(hasConnection(0) ? &getInputValue(0) : nullptr),
		(hasConnection(1) ? &getInputValue(1) : nullptr),
		(Embedded::MathNode::Types) m_values[0]
	};
	node.Evaluate(ImGui::GetIO().DeltaTime);
	m_outputs[0] = node.out;
}

void Node::evaluateMap() 
{
	Embedded::MapValueNode node = {
		(hasConnection(0) ? &getInputValue(0) : nullptr),
		m_values[0],
		m_values[1],
		m_values[2],
		m_values[3]
	};
	node.Evaluate(ImGui::GetIO().DeltaTime);
	m_outputs[0] = node.out;
}

void Node::evaulateCurve() 
{
	Embedded::CurveNode node = {
		(hasConnection(0) ? &getInputValue(0) : nullptr),
		(Embedded::CurveNode::Types) m_values[0],
		(bool) m_values[1],
		m_values[2],
		m_values[3],
		m_outputs[0]
	};
	node.Evaluate(ImGui::GetIO().DeltaTime);
	m_values[3] = node.timerCounter;
	m_outputs[0] = node.out;
}

void Node::evaluatePushButton() 
{
	Embedded::PushButtonNode node = {
		(hasConnection(0) ? &getInputValue(0) : nullptr),
		(hasConnection(1) ? &getInputValue(1) : nullptr),
		(hasConnection(2) ? &getInputValue(2) : nullptr),
		(bool) m_values[0],
		m_outputs[0],
		m_outputs[1]
	};
	node.Evaluate(ImGui::GetIO().DeltaTime);
	m_values[0] = node.lastButtonState;
	m_outputs[0] = node.ignitionOut;
	m_outputs[1] = node.starterOut;
}

void Node::evaluateSelect()
{
	Embedded::SelectNode node = {
		(hasConnection(0) ? &getInputValue(0) : nullptr),
		(hasConnection(1) ? &getInputValue(1) : nullptr),
		(hasConnection(2) ? &getInputValue(2) : nullptr),
		m_outputs[0]
	};
	node.Evaluate(ImGui::GetIO().DeltaTime);
	m_outputs[0] = node.out;

}

float& Node::getInputValue(int inputIndex) {
	return m_inputs[inputIndex].node.lock()->m_outputs[m_inputs[inputIndex].outputIndex];
}

bool Node::hasConnection(int inputIndex) {
	return !m_inputs[inputIndex].node.expired();
}
