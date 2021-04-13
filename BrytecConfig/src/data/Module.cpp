#include "Module.h"

std::string Module::typeNames[] = {
	"Input Module",
	"Power Module"
};

Module::Module(ModuleTypes type) 
	: m_type(type)
{
	int pinCount = 0;
	PinDirection direction;
	switch(m_type) {
		case ModuleTypes::Input:
			direction = PinDirection::Input;
			pinCount = 34;
			for(int i = 0; i < pinCount; i++)
				m_pins.emplace_back(std::make_shared<Pin>(direction, ALL_INPUT_TYPES));
			break;
		case ModuleTypes::Power:
			direction = PinDirection::Output;
			pinCount = 12;
			for(int i = 0; i < pinCount; i++)
				m_pins.emplace_back(std::make_shared<Pin>(direction, ALL_OUTPUT_TYPES));
			break;
	}


}
