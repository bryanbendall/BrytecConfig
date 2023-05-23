#include "UsbManager.h"

#include "AppManager.h"
#include "BrytecConfigEmbedded/EBrytecApp.h"
#include <chrono>
#include <iostream>

namespace Brytec {

UsbManager::UsbManager()
{
    m_usb.setReceiveCallback(std::bind(&UsbManager::onReceive, this, std::placeholders::_1));
    m_canBusCallback = std::bind(&CanBusStream::canBusReceived, &AppManager::getCanBusStream(), std::placeholders::_1);
    AppManager::getCanBusStream().setSendFunction(std::bind(&UsbManager::send, this, std::placeholders::_1));
}

UsbManager::~UsbManager()
{
    m_usb.close();
}

void UsbManager::update()
{
    static bool first = true;
    if (first) {
        m_device = AppManager::getLastSerialPort();
        first = false;
    }

    if (!m_usb.isOpen())
        return;

    AppManager::getCanBusStream().update();
}

bool UsbManager::isDeviceValid()
{
    std::vector<serial::PortInfo> ports = getAvailablePorts();
    auto ret = std::find_if(ports.begin(), ports.end(), [this](const serial::PortInfo& info) {
        return m_device.port == info.port;
    });

    return (ret != ports.end());
}

void UsbManager::open()
{
    m_usb.open(m_device.port);
    AppManager::setLastSerialPort(m_device);
}

void UsbManager::send(CanExtFrame& frame)
{
    UsbPacket packet;
    packet.set<CanExtFrame>(frame);
    m_usb.send(packet);
}

void UsbManager::onReceive(UsbPacket packet)
{
    switch (packet.getType()) {
    case UsbPacketType::Command:

        break;
    case UsbPacketType::Can: {
        m_canBusCallback(packet.as<CanExtFrame>());
    } break;
    case UsbPacketType::Status:

        break;
    case UsbPacketType::Unknown:
        std::cout << "Unknown Usb packet type!" << std::endl;
        break;
    }
}
}