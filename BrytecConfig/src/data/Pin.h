#pragma once

#include <deque>
#include "Selectable.h"
#include <memory>
#include <string>
#include "NodeGroup.h"
#include "IOTypes.h"

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

#define ALL_INPUT_TYPES std::vector<IOTypes::Types>{ IOTypes::Types::Input_12V, IOTypes::Types::Input_Ground, IOTypes::Types::Input_5V, IOTypes::Types::Input_5V_Variable, IOTypes::Types::Input_Can }
#define ALL_OUTPUT_TYPES std::vector<IOTypes::Types>{	IOTypes::Types::Output_12V_Pwm, IOTypes::Types::Output_12V, IOTypes::Types::Output_12V_Low_Current, IOTypes::Types::Output_Ground }

class Pin : public Selectable
{
	std::string m_pinoutName = "A1";
	PinCurrentLimit m_currentLimit = (PinCurrentLimit)-1;
	std::vector<IOTypes::Types> m_availableTypes;
	std::weak_ptr<NodeGroup> m_nodeGroup;

public:
	static const char* currentNames[(size_t) PinCurrentLimit::Count];

public:
	Pin(std::string pinoutName, std::vector<IOTypes::Types> availableTypes);
	~Pin();
	std::string& getPinoutName() { return m_pinoutName; }
	PinCurrentLimit& getCurrentLimit() { return m_currentLimit; }
	const std::vector<IOTypes::Types>& getAvailableTypes() { return m_availableTypes; }
	std::shared_ptr<NodeGroup> getNodeGroup() { return m_nodeGroup.lock(); }

};

