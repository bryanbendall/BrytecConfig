#pragma once

#include "Pin.h"

namespace Brytec {

class PhysicalPin : public Pin {

public:
    PhysicalPin(const std::string& pinoutName, const std::vector<IOTypes::Types>& availableTypes, uint8_t maxCurrent, bool pwm);
    virtual ~PhysicalPin();

    std::string& getPinoutName() { return m_pinoutName; }
    int8_t getMaxCurrent() { return m_maxCurrent; }
    void setMaxCurrent(int8_t current) { m_maxCurrent = current; }
    bool getPwm() { return m_pwm; }
    void setPwm(bool pwm) { m_pwm = pwm; }

private:
    std::string m_pinoutName = "A1";
    uint8_t m_maxCurrent = 0;
    bool m_pwm = false;
};

}