#pragma once

#include "Can/ECanBus.h"
#include <string>

namespace Brytec {

class CanBus {

public:
    CanBus() = default;

    std::string& getHiPinout() { return m_hiPinout; }
    std::string& getLoPinout() { return m_loPinout; }

    CanTypes::Types getType() { return m_type; }
    void setType(CanTypes::Types type);

    CanSpeed::Types getSpeed() { return m_speed; }
    void setSpeed(CanSpeed::Types speed) { m_speed = speed; }

private:
    std::string m_hiPinout = "Hi";
    std::string m_loPinout = "Lo";
    CanTypes::Types m_type = CanTypes::Types::Disabled;
    CanSpeed::Types m_speed = CanSpeed::Types::Speed_1MBps;
};
}