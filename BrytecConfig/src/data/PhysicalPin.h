#pragma once

#include "Pin.h"

class PhysicalPin : public Pin {

public:
    PhysicalPin(const std::string& pinoutName, const std::vector<IOTypes::Types>& availableTypes, uint8_t maxCurrent);
    virtual ~PhysicalPin();

    std::string& getPinoutName() { return m_pinoutName; }
    void setMaxCurrent(int8_t current) { m_maxCurrent = current; }
    int8_t getMaxCurrent() { return m_maxCurrent; }

private:
    std::string m_pinoutName = "A1";
    uint8_t m_maxCurrent = 0;
};