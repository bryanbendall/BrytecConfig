#include "UsbManager.h"

#include "BrytecConfigEmbedded/EBrytecApp.h"
#include <iostream>

namespace Brytec {

void UsbManager::update()
{
    if (!m_usb.isOpen())
        return;

    if (!m_doneSendingConfig && !m_waitForReturn) {
        if (m_canToSend.size() > 0) {
            send(m_canToSend.front());
            m_canToSend.pop_front();
            m_waitForReturn = true;
            m_amountLeftToSend = m_canToSend.size();
            if (m_canToSend.size() == 0)
                m_doneSendingConfig = true;
        }
    }

    std::vector<UsbPacket> newPackets = m_usb.getReceivedPackets();
    for (auto& packet : newPackets) {
        switch (packet.getType()) {
        case UsbPacketType::Command:

            break;
        case UsbPacketType::Can: {
            CanExtFrame frame = packet.as<CanExtFrame>();
            m_canFrames.push_back(frame);
            CanCommands command(frame);
            if (command.command == CanCommands::Ack) {
                m_waitForReturn = false;
            } else if (command.command == CanCommands::Nak)
                m_canToSend.clear();
        } break;
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

void UsbManager::sendConfig(uint8_t moduleAddress, std::vector<uint8_t>& data)
{
    CanCommands command;
    command.command = CanCommands::Command::ChangeMode;
    command.moduleAddress = moduleAddress;
    command.data[0] = EBrytecApp::Mode::Stopped;
    m_canToSend.push_back(command.getFrame());

    command.command = CanCommands::Command::ChangeMode;
    command.data[0] = EBrytecApp::Mode::Programming;
    m_canToSend.push_back(command.getFrame());

    command.command = CanCommands::Command::ReserveConfigSize;
    command.nodeGroupIndex = data.size();
    m_canToSend.push_back(command.getFrame());

    // write data
    command.command = CanCommands::Command::WriteConfigData;
    command.nodeGroupIndex = 0;
    uint16_t index;
    for (int i = 0; i < data.size(); i++) {
        index = i % 8;
        command.data[index] = data[i];

        if (index == 7) {
            m_canToSend.push_back(command.getFrame());
            command.nodeGroupIndex = i + 1;
        }
    }
    if (index != 7)
        m_canToSend.push_back(command.getFrame());

    command.command = CanCommands::Command::ChangeMode;
    command.data[0] = EBrytecApp::Mode::Stopped;
    m_canToSend.push_back(command.getFrame());

    command.command = CanCommands::Command::ReloadConfig;
    m_canToSend.push_back(command.getFrame());

    command.command = CanCommands::Command::ChangeMode;
    command.data[0] = EBrytecApp::Mode::Normal;
    m_canToSend.push_back(command.getFrame());

    m_amountToSend = m_canToSend.size();
    m_amountLeftToSend = m_amountToSend;
    m_doneSendingConfig = false;
}
}