#include "PhysicalPin.h"

namespace Brytec {

PhysicalPin::PhysicalPin(const std::string& pinoutName, const std::vector<IOTypes::Types>& availableTypes, uint8_t maxCurrent, bool pwm)
    : m_pinoutName(pinoutName)
    , m_maxCurrent(maxCurrent)
    , m_pwm(pwm)
{
    setAvailableTypes(availableTypes);
}

PhysicalPin::~PhysicalPin()
{
}

}