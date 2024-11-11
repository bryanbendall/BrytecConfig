#include "NetManager.h"

#include "AppManager.h"
#include "BrytecConfigEmbedded/EBrytecApp.h"
#include "communication/ModuleDebug.h"
#include <chrono>
#include <iostream>

namespace Brytec {

NetManager::NetManager()
{
    m_client.setReceiveCallback(std::bind(&NetManager::onReceive, this, std::placeholders::_1));
    m_canBusCallback = std::bind(&CanBusStream::canBusReceived, &AppManager::getCanBusStream(), std::placeholders::_1);
}

NetManager::~NetManager()
{
    m_client.close();
}

void NetManager::update()
{
    if (!m_client.isConnected())
        return;

    // if (m_usb.isOpen() && !isDeviceValid())
    // m_usb.close();

    AppManager::getCanBusStream().update();
}

void NetManager::open()
{
    AppManager::getCanBusStream().setSendFunction(std::bind(&NetManager::sendCan, this, std::placeholders::_1));
    // TODO be able to set ip and port
    m_client.startConnect("127.0.0.1", 6969);
}

void NetManager::sendCan(CanFrame& frame)
{
    UsbPacket packet;
    packet.set<CanFrame>(frame);
    m_client.send(packet);
}

void NetManager::sendCommand(ModuleCommand moduleCommand)
{
    UsbPacket packet;
    packet.set<ModuleCommand>(moduleCommand);
    m_client.send(packet);
}

void NetManager::onReceive(UsbPacket packet)
{
    switch (packet.getType()) {
    case UsbPacketType::Command:

        break;
    case UsbPacketType::Can: {
        m_canBusCallback(packet.as<CanFrame>());
    } break;
    case UsbPacketType::Status:

        break;
    case UsbPacketType::Unknown:
        std::cout << "Unknown Usb packet type!" << std::endl;
        break;
    case UsbPacketType::DebugPrint:
        // std::cout.write((char*)&packet.data[1], packet.length - 1);
        ModuleDebug::push((char*)&packet.data[1], packet.length - 1);
        break;
    }
}

}