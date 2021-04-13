#include "Node.h"
#include <iostream>

const char* Node::s_nodeName[(int) NodeTypes::Count] = {
	"Output",
	"Final Value",
	"Raw Value",
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

const char* Node::s_compareNames[(int) CompareTypes::Count] = {
	"Greater Than",
	"Greater Equal To",
	"Less Than",
	"Less Equal To"
};

const char* Node::s_mathNames[(int) MathTypes::Count] = {
	"Add",
	"Subtract",
	"Multiply",
	"Divide"
};

const char* Node::s_curveNames[(int) CurveTypes::Count] = {
	"Toggle",
	"Linear",
	"Expontial",
	"Breathing"
};

Node::Node(int id, ImVec2 position, NodeTypes type) : m_id(id), m_position(position), m_type(type), m_name(Node::s_nodeName[(int)type])
{
	int inputs = 0;
	int outputs = 0;
	int values = 0;

	switch (type)
	{
	case NodeTypes::Output:
		inputs = 1;
		break;
	case NodeTypes::Final_Input_Value:
		inputs = 1;
		break;
	case NodeTypes::Raw_Input_Value:
		outputs = 1;
		values = 1;
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

NodeConnection& Node::getInput(int index) 
{
	return m_inputs[index - m_outputs.size()];
}

unsigned int Node::getSize()
{
	if(m_type == NodeTypes::Output || m_type == NodeTypes::Final_Input_Value)
		return 4;

	unsigned int ret = 0;
	ret += 4; // char for type
	ret += m_outputs.size() * 4;
	ret += m_inputs.size() * 4;
	ret += m_values.size() * 4;
	return ret;
}

void Node::evaluate() {
	switch(m_type) {
		case NodeTypes::Output:											break;
		case NodeTypes::Final_Input_Value:								break;
		case NodeTypes::Raw_Input_Value:	m_outputs[0] = m_values[0]; break;
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
	for(size_t i = 0; i < m_inputs.size(); i++) {
		if(hasConnection(i) && getInputValue(i) <= 0.0f) {
			m_outputs[0] = 0.0f;
			return;
		}
	}
	m_outputs[0] = 1.0f;
}

void Node::evaluateOr() 
{
	for(size_t i = 0; i < m_inputs.size(); i++) {
		if(hasConnection(i) && getInputValue(i) > 0.0f) {
			m_outputs[0] = 1.0f;
			return;
		}
	}
	m_outputs[0] = 0.0f;
}

void Node::evaluateInvert() 
{
	if(hasConnection(0) && getInputValue(0) <= 0.0f) {
		m_outputs[0] = 1.0f;
		return;
	}
	m_outputs[0] = 0.0f;
}

void Node::evaluateTwoStage() 
{
	if(hasConnection(1) && getInputValue(1) > 0.0f) {
		m_outputs[0] = m_values[1];
		return;
	}
	else if(hasConnection(0) && getInputValue(0) > 0.0f) {
		m_outputs[0] = m_values[0];
		return;
	}
	m_outputs[0] = 0.0f;
}

void Node::evaluateOnOff() 
{
	if(hasConnection(1) && getInputValue(1) > 0.0f) {
		m_outputs[0] = 0.0f;
		return;
	} else if(hasConnection(0) && getInputValue(0) > 0.0f) {
		m_outputs[0] = 1.0f;
		return;
	}
	m_outputs[0] = 0.0f;
}

void Node::evaluateToggle() 
{
	if(!hasConnection(0)) {
		m_outputs[0] = 0.0f;
		return;
	}

	if(getInputValue(0) > 0.0f) {
		if(m_values[0] > 0.0f) {
			return;
		}
		if(m_outputs[0] <= 0.0f)
			m_outputs[0] = 1.0f;
		else
			m_outputs[0] = 0.0f;

		m_values[0] = getInputValue(0);
		return;
	} else {
		m_values[0] = 0.0f;
	}
}

void Node::evaluateDelay() 
{
	float& counter = m_values[1];

	if(!hasConnection(0)) {
		counter = 0.0f;
		m_outputs[0] = 0.0f;
		return;
	}

	if(getInputValue(0) > 0.0f) {
		counter += ImGui::GetIO().DeltaTime;
		if(counter >= m_values[0]) {
			m_outputs[0] = getInputValue(0);
			counter = m_values[0];
			return;
		}
	} else {
		counter = 0.0f;
		m_outputs[0] = 0.0f;
	}
}

void Node::evaluateCompare() 
{
	if(!hasConnection(0) || !hasConnection(1)) {
		m_outputs[0] = 0.0f;
		return;
	}

	CompareTypes type = (CompareTypes) m_values[0];
	switch(type) {
		case CompareTypes::Greater:			
			if(getInputValue(0) > getInputValue(1))
				m_outputs[0] = 1.0f;
			else
				m_outputs[0] = 0.0f;
			break;
		case CompareTypes::GreaterEqual:	
			if(getInputValue(0) >= getInputValue(1))
				m_outputs[0] = 1.0f;
			else
				m_outputs[0] = 0.0f;
			break;
		case CompareTypes::Less:			
			if(getInputValue(0) < getInputValue(1))
				m_outputs[0] = 1.0f;
			else
				m_outputs[0] = 0.0f;
			break;
		case CompareTypes::LessEqual:		
			if(getInputValue(0) <= getInputValue(1))
				m_outputs[0] = 1.0f;
			else
				m_outputs[0] = 0.0f;
			break;
	}
}

void Node::evaluateMath() 
{
	if(!hasConnection(0) || !hasConnection(1)) {
		m_outputs[0] = 0.0f;
		return;
	}

	MathTypes type = (MathTypes) m_values[0];
	switch(type) {
		case MathTypes::Add:
			m_outputs[0] = getInputValue(0) + getInputValue(1);
			break;
		case MathTypes::Subtract:
			m_outputs[0] = getInputValue(0) - getInputValue(1);
			break;
		case MathTypes::Multiply:
			m_outputs[0] = getInputValue(0) * getInputValue(1);
			break;
		case MathTypes::Divide:
			m_outputs[0] = getInputValue(0) / getInputValue(1);
			break;
	}
}

void Node::evaluateMap() 
{
	if(!hasConnection(0)) {
		m_outputs[0] = 0.0f;
		return;
	}

	float& x =  m_values[0];
	float& y =  m_values[1];
	float& x1 = m_values[2];
	float& y1 = m_values[3];
	float sloap = (y - y1) / (x - x1);

	m_outputs[0] = (sloap * (getInputValue(0) - x1)) + y1;
}

void Node::evaulateCurve() 
{
	float& repeat = m_values[1];
	float& onShutdown = m_values[2];
	float& timeout = m_values[3];
	float& lastIn = m_values[4];
	float& timerCounter = m_values[5];

	if(!hasConnection(0) || timeout <= 0.0f) {
		m_outputs[0] = 0.0f;
		return;
	}

	CurveTypes type = (CurveTypes) m_values[0];
	switch(type) {

		case CurveTypes::Toggle:
			if(getInputValue(0) > 0.0f) {
				timerCounter += ImGui::GetIO().DeltaTime;
			} else {
				timerCounter = 0;
				m_outputs[0] = 0.0f;
				return;
			}
			if(timerCounter >= timeout)
				timerCounter = timeout;

			if(getInputValue(0) > 0.0f && lastIn <= 0.0f) {
				m_outputs[0] = 100.0f;
				break;
			}
			if(getInputValue(0) > 0.0f && repeat <= 0.0f && m_outputs[0] <= 0.0f) {
				timerCounter = 0;
				break;
			}
			if(getInputValue(0) > 0.0f) {
				if(timerCounter >= timeout) {
					if(m_outputs[0] <= 0.0f)
						m_outputs[0] = 100.0f;
					else
						m_outputs[0] = 0.0f;
					timerCounter = 0;
				}
			}
			break;

		case CurveTypes::Linear:
			if(getInputValue(0) > 0.0f) {
				timerCounter += ImGui::GetIO().DeltaTime;
				if(repeat > 0.0f && m_outputs[0] >= 100.0f) 
					timerCounter = 0;
				
				if(timerCounter >= timeout) 
					timerCounter = timeout;
				
				m_outputs[0] = timerCounter / timeout * 100.0f;
				break;
			}

			if(getInputValue(0) <= 0.0f && onShutdown > 0.0f && m_outputs[0] > 0.0f) {
				timerCounter -= ImGui::GetIO().DeltaTime;
				m_outputs[0] = timerCounter / timeout * 100.0f;

			} else {
				m_outputs[0] = 0.0f;
				timerCounter = 0;
			}
			break;

		case CurveTypes::Exponential:

			if(getInputValue(0) > 0.0f) {
				float steps = 100.0f / (timeout * timeout);
				timerCounter += ImGui::GetIO().DeltaTime;
				if(repeat > 0.0f && m_outputs[0] >= 100.0f) {
					timerCounter = 0;
				}
				if(timerCounter >= timeout) {
					timerCounter = timeout;
				}
				
				m_outputs[0] = steps * timerCounter * timerCounter;
				break;
			} 

			if(getInputValue(0) <= 0.0f && onShutdown > 0.0f && m_outputs[0] > 0.1f) {
				float steps = 100.0f / (timeout * timeout);
				timerCounter -= ImGui::GetIO().DeltaTime;
				m_outputs[0] = steps * timerCounter * timerCounter;

			} else {
				m_outputs[0] = 0.0f;
				timerCounter = 0;
			}
			break;

		case CurveTypes::Breathing:
			if(getInputValue(0) > 0.0f) {
				timerCounter += ImGui::GetIO().DeltaTime;

				if(repeat > 0.0f && timerCounter >= timeout) {
					timerCounter = 0;
				} else if(timerCounter >= timeout) {
					timerCounter = timeout;
					m_outputs[0] = 0.0f;
					return;
				}

				float gamma = 0.20f; // affects the width of peak (more or less darkness)
				float beta = 0.5f;
				m_outputs[0] = 100.0f * (exp(-(pow(((timerCounter / timeout) - beta) / gamma, 2.0f)) / 2.0f));

			} else {
				m_outputs[0] = 0.0f;
				timerCounter = 0;
			}
			break;
	}

	if(getInputValue(0))
		lastIn = getInputValue(0);
}

void Node::evaluatePushButton() 
{
	float& pushButtonIn = getInputValue(0);
	float& brakeIn = getInputValue(1);
	float& engineRunningIn = getInputValue(2);
	float& ignitionOut = m_outputs[0];
	float& starterOut = m_outputs[1];
	float& lastEngineRunning = m_values[0];

	if(!hasConnection(0) || !hasConnection(1)) {
		ignitionOut = 0.0f;
		starterOut = 0.0f;
		return;
	}

	float lastEngingRunning = 0.0f;
	if(lastEngingRunning <= 0.0f && engineRunningIn > 0.0f && starterOut > 0.0f) {
		starterOut = 0.0f;
		return;
	}
	lastEngingRunning = engineRunningIn;

	static float lastPushButton = 0.0f;
	if(lastPushButton > 0.0f && pushButtonIn > 0.0f) {
		return;
	}
	lastPushButton = pushButtonIn;

	if(ignitionOut <= 0.0f && pushButtonIn > 0.0f) {
		ignitionOut = 1.0f;
		starterOut = 0.0f;
		return;
	}

	if(pushButtonIn > 0.0f && engineRunningIn <= 0.0f && brakeIn <= 0.0f) {
		ignitionOut = 0.0f;
		starterOut = 0.0f;
		return;
	}

	if(pushButtonIn > 0.0f && engineRunningIn > 0.0f) {
		ignitionOut = 0.0f;
		starterOut = 0.0f;
		return;
	}


	if(pushButtonIn > 0.0f && engineRunningIn <= 0.0f && brakeIn > 0.0f) {
		starterOut = 1.0f;
		return;
	}

	starterOut = 0.0f;
}

float& Node::getInputValue(int inputIndex) {
	return m_inputs[inputIndex].node.lock()->m_outputs[m_inputs[inputIndex].outputIndex];
}

bool Node::hasConnection(int inputIndex) {
	return !m_inputs[inputIndex].node.expired();
}
