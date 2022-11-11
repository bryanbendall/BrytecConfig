#include "PhysicalPin.h"

const char* PhysicalPin::currentNames[] = {
    "5 Amps",
    "7 Amps",
    "9 Amps",
    "11 Amps",
    "13 Amps",
    "15 Amps",
    "17 Amps"
};

PhysicalPin::PhysicalPin(const std::string& pinoutName, const std::vector<IOTypes::Types>& availableTypes)
    : m_pinoutName(pinoutName)
{
    setAvailableTypes(availableTypes);
}

PhysicalPin::~PhysicalPin()
{
}