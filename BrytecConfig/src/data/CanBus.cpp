#include "CanBus.h"

#include "EBrytecConfig.h"

namespace Brytec {

void CanBus::setType(CanTypes::Types type)
{
    m_type = type;

    switch (type) {
    case CanTypes::Types::Brytec:
        m_speed = DEFAULT_BRYTEC_CAN_SPEED;
        break;
    case CanTypes::Types::Holley:
        m_speed = CanSpeed::Types::Speed_1MBps;
        break;
    case CanTypes::Types::Racepak:
        m_speed = CanSpeed::Types::Speed_250kBps;
        break;

    default:
        break;
    }
}
}