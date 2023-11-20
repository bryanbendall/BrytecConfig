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

    default:
        break;
    }
}
}