#include "Node.h"
#include <iostream>


const char* Node::s_nodeName[(int) NodeTypes::Count] = {
	"Initial Value",
	"Final Value",
	"Pin",
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

Node::Node(int id, ImVec2 position, NodeTypes type) 
	: m_id(id), m_position(position), m_type(type), m_name(Node::s_nodeName[(int)type])
{
	int inputs = 0;
	int outputs = 0;
	int values = 0;

	switch (type)
	{
	case NodeTypes::Initial_Value:
		outputs = 1;
		values = 1;
		break;
	case NodeTypes::Final_Value:
		inputs = 1;
		break;
	case NodeTypes::Pin:
		outputs = 1;
		values =  1;
		break;
	case NodeTypes::And:
		inputs =  5;
		outputs = 1;
		break;
	case NodeTypes::Or:
		inputs =  5;
		outputs = 1;
		break;
	case NodeTypes::Two_Stage:
		inputs =  2;
		outputs = 1;
		values =  2;
		break;
	case NodeTypes::Curve:
		inputs = 1;
		outputs = 1;
		values = 6;
		break;
	case NodeTypes::Compare:
		inputs = 2;
		outputs = 1;
		values = 1;
		break;
	case NodeTypes::On_Off:
		inputs = 2;
		outputs = 1;
		break;
	case NodeTypes::Invert:
		inputs = 1;
		outputs = 1;
		break;
	case NodeTypes::Toggle:
		inputs = 1;
		outputs = 1;
		values = 1;
		break;
	case NodeTypes::Delay:
		inputs = 1;
		outputs = 1;
		values = 2;
		break;
	case NodeTypes::Push_Button:
		inputs = 3;
		outputs = 2;
		values = 1;
		break;
	case NodeTypes::Map_Value:
		inputs =  1;
		outputs = 1;
		values =  4;
		break;
	case NodeTypes::Math:
		inputs = 2;
		outputs = 1;
		values = 1;
		break;
	case NodeTypes::Value:
		outputs = 1;
		values = 1;
		break;
	case NodeTypes::Count:
		break;
	default:
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
		case NodeTypes::Initial_Value:		m_outputs[0] = m_values[0];	break;
		case NodeTypes::Final_Value:									break;
		case NodeTypes::Pin:				m_outputs[0] = m_values[0]; break;
		case NodeTypes::And:				evaluateAnd();				break;
		case NodeTypes::Or:					evaluateOr();				break;
		case NodeTypes::Two_Stage:			evaluateTwoStage();			break;
		case NodeTypes::Curve:				evaulateCurve();			break;
		case NodeTypes::Compare:			evaluateCompare();			break;
		case NodeTypes::On_Off:				evaluateOnOff();			break;
		case NodeTypes::Invert:				evaluateInvert();			break;
		case NodeTypes::Toggle:				evaluateToggle();			break;
		case NodeTypes::Delay:				evaluateDelay();			break;
		case NodeTypes::Push_Button:		evaluatePushButton();		break;
		case NodeTypes::Map_Value:			evaluateMap();				break;
		case NodeTypes::Math:				evaluateMath();				break;
		case NodeTypes::Value:				m_outputs[0] = m_values[0];	break;
		case NodeTypes::Count:											break;
		default:							assert(false);				break;
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
	Embedded::Evaluate(node, ImGui::GetIO().DeltaTime);
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
	Embedded::Evaluate(node, ImGui::GetIO().DeltaTime);
	m_outputs[0] = node.out;
}

void Node::evaluateInvert() 
{
	Embedded::InvertNode node = {
	(hasConnection(0) ? &getInputValue(0) : nullptr)
	};
	Embedded::Evaluate(node, ImGui::GetIO().DeltaTime);
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
	Embedded::Evaluate(node, ImGui::GetIO().DeltaTime);
	m_outputs[0] = node.out;
}

void Node::evaluateOnOff() 
{
	Embedded::OnOffNode node = {
		(hasConnection(0) ? &getInputValue(0) : nullptr),
		(hasConnection(1) ? &getInputValue(1) : nullptr)
	};
	Embedded::Evaluate(node, ImGui::GetIO().DeltaTime);
	m_outputs[0] = node.out;
}

void Node::evaluateToggle() 
{
	Embedded::ToggleNode node = {
		(hasConnection(0) ? &getInputValue(0) : nullptr),
		(bool)m_values[0],
		m_outputs[0]
	};
	Embedded::Evaluate(node, ImGui::GetIO().DeltaTime);
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
	Embedded::Evaluate(node, ImGui::GetIO().DeltaTime);
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
	Embedded::Evaluate(node, ImGui::GetIO().DeltaTime);
	m_outputs[0] = node.out;
}

void Node::evaluateMath() 
{
	Embedded::MathNode node = {
		(hasConnection(0) ? &getInputValue(0) : nullptr),
		(hasConnection(1) ? &getInputValue(1) : nullptr),
		(Embedded::MathNode::Types) m_values[0]
	};
	Embedded::Evaluate(node, ImGui::GetIO().DeltaTime);
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
	Embedded::Evaluate(node, ImGui::GetIO().DeltaTime);
	m_outputs[0] = node.out;
}

void Node::evaulateCurve() 
{
	Embedded::CurveNode node = {
		(hasConnection(0) ? &getInputValue(0) : nullptr),
		(Embedded::CurveNode::Types) m_values[0],
		(bool) m_values[1],
		(bool) m_values[2],
		(bool) m_values[4],
		m_values[3],
		m_values[5],
		m_outputs[0]
	};
	Embedded::Evaluate(node, ImGui::GetIO().DeltaTime);
	m_values[1] = node.repeat;
	m_values[2] = node.onShutdown;
	m_values[4] = node.lastIn;
	m_values[3] = node.timeout;
	m_values[5] = node.timerCounter;
	m_outputs[0] = node.out;

	//float& repeat = m_values[1];
	//float& onShutdown = m_values[2];
	//float& timeout = m_values[3];
	//float& lastIn = m_values[4];
	//float& timerCounter = m_values[5];

	//if(!hasConnection(0) || timeout <= 0.0f) {
	//	m_outputs[0] = 0.0f;
	//	return;
	//}

	//CurveTypes type = (CurveTypes) m_values[0];
	//switch(type) {

	//	case CurveTypes::Toggle:
	//		if(getInputValue(0) > 0.0f) {
	//			timerCounter += ImGui::GetIO().DeltaTime;
	//		} else {
	//			timerCounter = 0;
	//			m_outputs[0] = 0.0f;
	//			return;
	//		}
	//		if(timerCounter >= timeout)
	//			timerCounter = timeout;

	//		if(getInputValue(0) > 0.0f && lastIn <= 0.0f) {
	//			m_outputs[0] = 100.0f;
	//			break;
	//		}
	//		if(getInputValue(0) > 0.0f && repeat <= 0.0f && m_outputs[0] <= 0.0f) {
	//			timerCounter = 0;
	//			break;
	//		}
	//		if(getInputValue(0) > 0.0f) {
	//			if(timerCounter >= timeout) {
	//				if(m_outputs[0] <= 0.0f)
	//					m_outputs[0] = 100.0f;
	//				else
	//					m_outputs[0] = 0.0f;
	//				timerCounter = 0;
	//			}
	//		}
	//		break;

	//	case CurveTypes::Linear:
	//		if(getInputValue(0) > 0.0f) {
	//			timerCounter += ImGui::GetIO().DeltaTime;
	//			if(repeat > 0.0f && m_outputs[0] >= 100.0f) 
	//				timerCounter = 0;
	//			
	//			if(timerCounter >= timeout) 
	//				timerCounter = timeout;
	//			
	//			m_outputs[0] = timerCounter / timeout * 100.0f;
	//			break;
	//		}

	//		if(getInputValue(0) <= 0.0f && onShutdown > 0.0f && m_outputs[0] > 0.0f) {
	//			timerCounter -= ImGui::GetIO().DeltaTime;
	//			m_outputs[0] = timerCounter / timeout * 100.0f;

	//		} else {
	//			m_outputs[0] = 0.0f;
	//			timerCounter = 0;
	//		}
	//		break;

	//	case CurveTypes::Exponential:

	//		if(getInputValue(0) > 0.0f) {
	//			float steps = 100.0f / (timeout * timeout);
	//			timerCounter += ImGui::GetIO().DeltaTime;
	//			if(repeat > 0.0f && m_outputs[0] >= 100.0f) {
	//				timerCounter = 0;
	//			}
	//			if(timerCounter >= timeout) {
	//				timerCounter = timeout;
	//			}
	//			
	//			m_outputs[0] = steps * timerCounter * timerCounter;
	//			break;
	//		} 

	//		if(getInputValue(0) <= 0.0f && onShutdown > 0.0f && m_outputs[0] > 0.1f) {
	//			float steps = 100.0f / (timeout * timeout);
	//			timerCounter -= ImGui::GetIO().DeltaTime;
	//			m_outputs[0] = steps * timerCounter * timerCounter;

	//		} else {
	//			m_outputs[0] = 0.0f;
	//			timerCounter = 0;
	//		}
	//		break;

	//	case CurveTypes::Breathing:
	//		if(getInputValue(0) > 0.0f) {
	//			timerCounter += ImGui::GetIO().DeltaTime;

	//			if(repeat > 0.0f && timerCounter >= timeout) {
	//				timerCounter = 0;
	//			} else if(timerCounter >= timeout) {
	//				timerCounter = timeout;
	//				m_outputs[0] = 0.0f;
	//				return;
	//			}

	//			float gamma = 0.20f; // affects the width of peak (more or less darkness)
	//			float beta = 0.5f;
	//			m_outputs[0] = 100.0f * (exp(-(pow(((timerCounter / timeout) - beta) / gamma, 2.0f)) / 2.0f));

	//		} else {
	//			m_outputs[0] = 0.0f;
	//			timerCounter = 0;
	//		}
	//		break;
	//}

	//if(getInputValue(0))
	//	lastIn = getInputValue(0);
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
	Embedded::Evaluate(node, ImGui::GetIO().DeltaTime);
	m_values[0] = node.lastButtonState;
	m_outputs[0] = node.ignitionOut;
	m_outputs[1] = node.starterOut;
}

float& Node::getInputValue(int inputIndex) {
	return m_inputs[inputIndex].node.lock()->m_outputs[m_inputs[inputIndex].outputIndex];
}

bool Node::hasConnection(int inputIndex) {
	return !m_inputs[inputIndex].node.expired();
}
