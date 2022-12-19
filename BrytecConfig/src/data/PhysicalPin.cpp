#include "PhysicalPin.h"

PhysicalPin::PhysicalPin(const std::string& pinoutName, const std::vector<IOTypes::Types>& availableTypes, uint8_t maxCurrent)
    : m_pinoutName(pinoutName)
    , m_maxCurrent(maxCurrent)
{
    setAvailableTypes(availableTypes);
}

PhysicalPin::~PhysicalPin()
{
}