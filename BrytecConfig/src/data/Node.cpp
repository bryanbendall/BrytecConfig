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
	"Switch"
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

	switch (type)
	{
	case Embedded::NodeTypes::Initial_Value:
		m_outputs.push_back(0.0f);
		m_values.push_back(0.0f);
		break;
	case Embedded::NodeTypes::Final_Value:
		m_inputs.push_back(NodeConnection());
		break;
	case Embedded::NodeTypes::Node_Group:
		m_outputs.push_back(0.0f);
		m_values.push_back(0.0f);
		break;
	case Embedded::NodeTypes::And:
		m_inputs.push_back(NodeConnection(1.0f));	// Input
		m_inputs.push_back(NodeConnection(1.0f));	// Input
		m_inputs.push_back(NodeConnection(1.0f));	// Input
		m_inputs.push_back(NodeConnection(1.0f));	// Input
		m_inputs.push_back(NodeConnection(1.0f));	// Input
		m_outputs.push_back(0.0f);					// Output
		break;
	case Embedded::NodeTypes::Or:
		m_inputs.push_back(NodeConnection());		// Input
		m_inputs.push_back(NodeConnection());		// Input
		m_inputs.push_back(NodeConnection());		// Input
		m_inputs.push_back(NodeConnection());		// Input
		m_inputs.push_back(NodeConnection());		// Input
		m_outputs.push_back(0.0f);					// Output
		break;
	case Embedded::NodeTypes::Two_Stage:
		m_inputs.push_back(NodeConnection());		// Stage 1 trig
		m_inputs.push_back(NodeConnection(50.0f));	// Stage 1 value
		m_inputs.push_back(NodeConnection());		// Stage 2 trig
		m_inputs.push_back(NodeConnection(100.0f));	// Stage 2 value
		m_outputs.push_back(0.0f);					// Output
		break;
	case Embedded::NodeTypes::Curve:
		m_inputs.push_back(NodeConnection());		// Input
		m_inputs.push_back(NodeConnection());		// Repeat
		m_inputs.push_back(NodeConnection());		// Time
		m_outputs.push_back(0.0f);					// Output
		m_values.push_back(0.0f);					// Curve type
		m_values.push_back(0.0f);					// Internet time counter
		break;
	case Embedded::NodeTypes::Compare:
		m_inputs.push_back(NodeConnection());		// Value 1
		m_inputs.push_back(NodeConnection());		// Value 2
		m_outputs.push_back(0.0f);					// Output
		m_values.push_back(0.0f);					// Compare type
		break;
	case Embedded::NodeTypes::On_Off:
		m_inputs.push_back(NodeConnection());		// On
		m_inputs.push_back(NodeConnection());		// Off
		m_outputs.push_back(0.0f);					// Output
		break;
	case Embedded::NodeTypes::Invert:
		m_inputs.push_back(NodeConnection());		// Input
		m_outputs.push_back(0.0f);					// Output
		break;
	case Embedded::NodeTypes::Toggle:
		m_inputs.push_back(NodeConnection());		// Input
		m_outputs.push_back(0.0f);					// Output
		m_values.push_back(0.0f);					// Internal last value
		break;
	case Embedded::NodeTypes::Delay:
		m_inputs.push_back(NodeConnection());		// Input
		m_inputs.push_back(NodeConnection(1.0f));	// Delay time
		m_outputs.push_back(0.0f);					// Output
		m_values.push_back(0.0f);					// Internal timer
		break;
	case Embedded::NodeTypes::Push_Button:
		m_inputs.push_back(NodeConnection());		// Button
		m_inputs.push_back(NodeConnection());		// Neutral Safety
		m_inputs.push_back(NodeConnection());		// Engine running
		m_outputs.push_back(0.0f);					// Ignition output
		m_outputs.push_back(0.0f);					// Starter output
		m_values.push_back(0.0f);					// Internal state
		break;
	case Embedded::NodeTypes::Map_Value:
		m_inputs.push_back(NodeConnection());		// Value
		m_inputs.push_back(NodeConnection());		// From min
		m_inputs.push_back(NodeConnection(1.0f));	// From max
		m_inputs.push_back(NodeConnection());		// To min
		m_inputs.push_back(NodeConnection(100.0f));	// To max
		m_outputs.push_back(0.0f);					// Output
		break;
	case Embedded::NodeTypes::Math:
		m_inputs.push_back(NodeConnection());
		m_inputs.push_back(NodeConnection());
		m_outputs.push_back(0.0f);
		m_values.push_back(0.0f);
		break;
	case Embedded::NodeTypes::Value:
		m_outputs.push_back(0.0f);
		m_values.push_back(0.0f);
		break;
	case Embedded::NodeTypes::Switch:
		m_inputs.push_back(NodeConnection());
		m_inputs.push_back(NodeConnection());
		m_inputs.push_back(NodeConnection());
		m_outputs.push_back(0.0f);
		break;
	case Embedded::NodeTypes::Count:
		assert(false);
		break;
	default:
		assert(false);
		break;
	}

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

float& Node::getInputValue(int inputIndex)
{
	if(hasConnection(inputIndex))
		return m_inputs[inputIndex].ConnectedNode.lock()->m_outputs[m_inputs[inputIndex].OutputIndex];
	else
		return m_inputs[inputIndex].DefaultValue;
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
		case Embedded::NodeTypes::Switch:				evaluateSwitch();			break;
		case Embedded::NodeTypes::Count:											break;
		default:										assert(false);				break;
	}
}

void Node::evaluateAnd() 
{
	Embedded::AndNode node;
	Embedded::NodeInputMask mask;

	node.SetValue(0, mask, getInputValue(0));
	node.SetValue(1, mask, getInputValue(1));
	node.SetValue(2, mask, getInputValue(2));
	node.SetValue(3, mask, getInputValue(3));
	node.SetValue(4, mask, getInputValue(4));

	node.Evaluate(mask, ImGui::GetIO().DeltaTime);
	m_outputs[0] = node.m_out;
}

void Node::evaluateOr() 
{
	Embedded::OrNode node;
	Embedded::NodeInputMask mask;

	node.SetValue(0, mask, getInputValue(0));
	node.SetValue(1, mask, getInputValue(1));
	node.SetValue(2, mask, getInputValue(2));
	node.SetValue(3, mask, getInputValue(3));
	node.SetValue(4, mask, getInputValue(4));

	node.Evaluate(mask, ImGui::GetIO().DeltaTime);
	m_outputs[0] = node.m_out;
}

void Node::evaluateInvert() 
{
	Embedded::InvertNode node;
	Embedded::NodeInputMask mask;

	node.SetValue(0, mask, getInputValue(0));

	node.Evaluate(mask, ImGui::GetIO().DeltaTime);
	m_outputs[0] = node.m_out;
}

void Node::evaluateTwoStage() 
{
	Embedded::TwoStageNode node;
	Embedded::NodeInputMask mask;

	node.SetValue(0, mask, getInputValue(0));
	node.SetValue(1, mask, getInputValue(1));
	node.SetValue(2, mask, getInputValue(2));
	node.SetValue(3, mask, getInputValue(3));

	node.Evaluate(mask, ImGui::GetIO().DeltaTime);
	m_outputs[0] = node.m_out;
}

void Node::evaluateOnOff() 
{
	Embedded::OnOffNode node;
	Embedded::NodeInputMask mask;

	node.SetValue(0, mask, getInputValue(0));
	node.SetValue(1, mask, getInputValue(1));

	node.Evaluate(mask, ImGui::GetIO().DeltaTime);
	m_outputs[0] = node.m_out;
}

void Node::evaluateToggle() 
{
	Embedded::ToggleNode node;
	Embedded::NodeInputMask mask;

	node.SetValue(0, mask, getInputValue(0));
	node.SetValue(1, mask, getValue(0));
	node.m_out = m_outputs[0];

	node.Evaluate(mask, ImGui::GetIO().DeltaTime);
	m_values[0] = node.m_lastValue.value;
	m_outputs[0] = node.m_out;
}

void Node::evaluateDelay() 
{
	Embedded::DelayNode node;
	Embedded::NodeInputMask mask;

	node.SetValue(0, mask, getInputValue(0));
	node.SetValue(1, mask, getInputValue(1));
	node.SetValue(2, mask, getValue(0));

	node.Evaluate(mask, ImGui::GetIO().DeltaTime);
	m_values[1] = node.m_counter.value;
	m_outputs[0] = node.m_out;
}

void Node::evaluateCompare() 
{
	Embedded::CompareNode node;
	Embedded::NodeInputMask mask;

	node.SetValue(0, mask, getInputValue(0));
	node.SetValue(1, mask, getInputValue(1));
	node.SetValue(2, mask, getValue(0));

	node.Evaluate(mask, ImGui::GetIO().DeltaTime);
	m_outputs[0] = node.m_out;
}

void Node::evaluateMath() 
{
	Embedded::MathNode node;
	Embedded::NodeInputMask mask;

	node.SetValue(0, mask, getInputValue(0));
	node.SetValue(1, mask, getInputValue(1));
	node.SetValue(2, mask, getValue(0));

	node.Evaluate(mask, ImGui::GetIO().DeltaTime);
	m_outputs[0] = node.m_out;
}

void Node::evaluateMap() 
{
	Embedded::MapValueNode node;
	Embedded::NodeInputMask mask;

	node.SetValue(0, mask, getInputValue(0));
	node.SetValue(1, mask, getInputValue(1));
	node.SetValue(2, mask, getInputValue(2));
	node.SetValue(3, mask, getInputValue(3));
	node.SetValue(4, mask, getInputValue(4));

	node.Evaluate(mask, ImGui::GetIO().DeltaTime);
	m_outputs[0] = node.m_out;
}

void Node::evaulateCurve() 
{
	Embedded::CurveNode node;
	Embedded::NodeInputMask mask;

	node.SetValue(0, mask, getInputValue(0));
	node.SetValue(1, mask, getInputValue(1));
	node.SetValue(2, mask, getInputValue(2));
	node.SetValue(3, mask, getValue(0));
	node.SetValue(4, mask, getValue(1));

	node.Evaluate(mask, ImGui::GetIO().DeltaTime);
	m_values[1] = node.m_timerCounter.value;
	m_outputs[0] = node.m_out;
}

void Node::evaluatePushButton() 
{
	Embedded::PushButtonNode node;
	Embedded::NodeInputMask mask;

	node.SetValue(0, mask, getInputValue(0));
	node.SetValue(1, mask, getInputValue(1));
	node.SetValue(2, mask, getInputValue(2));
	node.SetValue(3, mask, getValue(0));
	node.m_ignitionOut = m_outputs[0];
	node.m_starterOut = m_outputs[1];

	node.Evaluate(mask, ImGui::GetIO().DeltaTime);
	m_values[0] = node.m_lastButtonState.value;
	m_outputs[0] = node.m_ignitionOut;
	m_outputs[1] = node.m_starterOut;
}

void Node::evaluateSwitch()
{
	Embedded::SwitchNode node;
	Embedded::NodeInputMask mask;

	node.SetValue(0, mask, getInputValue(0));
	node.SetValue(1, mask, getInputValue(1));
	node.SetValue(2, mask, getInputValue(2));
	node.m_out = m_outputs[0];

	node.Evaluate(mask, ImGui::GetIO().DeltaTime);
	m_outputs[0] = node.m_out;

}

bool Node::hasConnection(int inputIndex) {
	return !m_inputs[inputIndex].ConnectedNode.expired();
}
