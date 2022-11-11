#pragma once

#include "Pin.h"

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

class PhysicalPin : public Pin {

public:
    PhysicalPin(const std::string& pinoutName, const std::vector<IOTypes::Types>& availableTypes);
    virtual ~PhysicalPin();

    std::string& getPinoutName() { return m_pinoutName; }
    PinCurrentLimit& getCurrentLimit() { return m_currentLimit; }

public:
    static const char* currentNames[(size_t)PinCurrentLimit::Count];

private:
    std::string m_pinoutName = "A1";
    PinCurrentLimit m_currentLimit = (PinCurrentLimit)-1;
};