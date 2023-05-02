#include "UsbManager.h"

#include <iostream>

namespace Brytec {

void UsbManager::update()
{
    if (!m_usb.isOpen())
        return;

    std::vector<UsbPacket> newPackets = m_usb.getReceivedPackets();
    for (auto& packet : newPackets) {
        switch (packet.getType()) {
        case UsbPacketType::Command:

            break;
        case UsbPacketType::Can:
            m_canFrames.push_back(packet.as<CanExtFrame>());
            break;
        case UsbPacketType::Status:

            break;
        case UsbPacketType::Unknown:
            std::cout << "Unknown Usb packet type!" << std::endl;
            break;
        }
    }
}

void UsbManager::send(CanExtFrame& frame)
{
    UsbPacket packet;
    packet.set<CanExtFrame>(frame);
    m_usb.send(packet);
}

std::vector<CanExtFrame> UsbManager::getCanFrames()
{
    auto ret = m_canFrames;
    m_canFrames.clear();
    return ret;
}
}