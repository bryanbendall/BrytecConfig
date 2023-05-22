#include "CanBusStream.h"

#include "AppManager.h"
#include <iostream>

namespace Brytec {

void CanBusStream::update()
{
    if (!m_sending)
        return;

    m_timer += ImGui::GetIO().DeltaTime;

    if (m_timer > 1.0f) {

        if (m_retires >= s_maxRetries) {
            m_commandsToSend.clear();
            m_sending = false;
            m_callbackData.error = true;
            m_callback(m_callbackData);
        } else {
            m_retires++;
            m_sendFunction(m_commandsToSend.front());
        }
    }
}

void CanBusStream::requestModuleStatus(uint8_t moduleAddress)
{
    if (m_sending)
        return;

    m_toModuleAddress = moduleAddress;

    CanCommands command;
    command.moduleAddress = moduleAddress;
    command.command = CanCommands::Command::RequestStatus;
    m_commandsToSend.push_back(command.getFrame());
}

void CanBusStream::requestNodeGroupStatus(uint8_t moduleAddress, uint16_t nodeGroupIndex)
{
    if (m_sending)
        return;

    m_toModuleAddress = moduleAddress;

    CanCommands command;
    command.moduleAddress = moduleAddress;
    command.nodeGroupIndex = nodeGroupIndex;
    command.command = CanCommands::Command::RequestStatus;
    m_commandsToSend.push_back(command.getFrame());
}

void CanBusStream::changeMode(uint8_t moduleAddress, EBrytecApp::Mode mode)
{
    if (m_sending)
        return;

    m_toModuleAddress = moduleAddress;

    CanCommands command;
    command.moduleAddress = moduleAddress;
    command.command = CanCommands::Command::ChangeMode;
    command.data[0] = mode;
    m_commandsToSend.push_back(command.getFrame());
}

void CanBusStream::reloadConfig(uint8_t moduleAddress)
{
    if (m_sending)
        return;

    m_toModuleAddress = moduleAddress;

    changeMode(moduleAddress, EBrytecApp::Mode::Stopped);

    CanCommands command;
    command.moduleAddress = moduleAddress;
    command.command = CanCommands::Command::ReloadConfig;
    m_commandsToSend.push_back(command.getFrame());

    changeMode(moduleAddress, EBrytecApp::Mode::Normal);
}

void CanBusStream::reserveConfigSize(uint8_t moduleAddress, uint16_t size)
{
    if (m_sending)
        return;

    m_toModuleAddress = moduleAddress;

    CanCommands command;
    command.moduleAddress = moduleAddress;
    command.command = CanCommands::Command::ReserveConfigSize;
    command.nodeGroupIndex = size;
    m_commandsToSend.push_back(command.getFrame());
}

void CanBusStream::sendNewConfig(uint8_t moduleAddress, std::vector<uint8_t>& data)
{
    if (m_sending)
        return;

    m_toModuleAddress = moduleAddress;

    changeMode(moduleAddress, EBrytecApp::Mode::Stopped);
    changeMode(moduleAddress, EBrytecApp::Mode::Programming);
    reserveConfigSize(moduleAddress, data.size());

    // write data
    CanCommands command;
    command.moduleAddress = moduleAddress;
    command.command = CanCommands::Command::WriteConfigData;
    command.nodeGroupIndex = 0;
    uint16_t index;
    for (int i = 0; i < data.size(); i++) {
        index = i % 8;
        command.data[index] = data[i];

        if (index == 7) {
            m_commandsToSend.push_back(command.getFrame());
            command.nodeGroupIndex = i + 1;
        }
    }
    if (index != 7)
        m_commandsToSend.push_back(command.getFrame());

    reloadConfig(moduleAddress);
}

void CanBusStream::send(std::function<void(CanBusStreamCallbackData)> callback)
{
    if (m_sending)
        return;

    m_callbackData.total = m_commandsToSend.size();
    m_callbackData.leftToSend = m_callbackData.total;
    m_callbackData.error = false;
    m_callback = callback;
    m_sending = true;

    m_timer = 0.0f;
    m_retires = 0;

    if (m_commandsToSend.size() > 0)
        m_sendFunction(m_commandsToSend.front());
}

void CanBusStream::canBusReceived(CanExtFrame frame)
{
    if (frame.isBroadcast()) {

        auto it = std::find_if(m_nodeGroupStatuses.begin(), m_nodeGroupStatuses.end(), [frame](PinStatusBroadcast& bc) {
            PinStatusBroadcast frameBc(frame);
            return (frameBc.moduleAddress == bc.moduleAddress && frameBc.nodeGroupIndex == bc.nodeGroupIndex);
        });

        if (it != m_nodeGroupStatuses.end())
            *it = frame;
        else
            m_nodeGroupStatuses.push_back(frame);

        return;
    }

    CanCommands command(frame);

    bool isCorrectModule = m_toModuleAddress == CanCommands::AllModules || m_toModuleAddress == command.moduleAddress;

    switch (command.command) {

    case CanCommands::Command::Nak:
        if (!isCorrectModule)
            return;
        m_commandsToSend.clear();
        m_callbackData.error = true;
        break;

    case CanCommands::Command::Ack:
        if (!isCorrectModule)
            return;
        if (m_commandsToSend.size() > 0)
            m_commandsToSend.pop_front();
        if (m_commandsToSend.size() > 0)
            m_sendFunction(m_commandsToSend.front());
        break;

    case CanCommands::Command::SendStatus:
        if (m_commandsToSend.size() > 0 && isCorrectModule)
            m_commandsToSend.pop_front();
        m_moduleStatuses[command.moduleAddress] = {
            command.moduleAddress,
            (bool)command.data[1],
            (EBrytecApp::Mode)command.data[0]
        };
        break;

    default:
        std::cout << "Unknow can command" << std::endl;
        break;
    }

    m_timer = 0.0f;
    m_retires = 0;

    if (m_commandsToSend.size() == 0)
        m_sending = false;

    m_callbackData.leftToSend = m_commandsToSend.size();
    m_callback(m_callbackData);
}

}