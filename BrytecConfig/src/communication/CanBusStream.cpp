#include "CanBusStream.h"

#include "AppManager.h"
#include "Deserializer/BinaryArrayDeserializer.h"
#include "Deserializer/BinaryBufferSerializer.h"
#include <iostream>

namespace Brytec {

void CanBusStream::update()
{
    if (m_commandsToSend.size() == 0)
        m_sending = false;

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
            sendNextFrame();
        }
    }
}

void CanBusStream::requestModuleStatus(uint8_t moduleAddress)
{
    if (m_sending)
        return;

    m_waitForReply = false;

    m_toModuleAddress = moduleAddress;

    CanCommands command;
    command.moduleAddress = moduleAddress;
    command.command = CanCommands::Command::RequestStatus;
    m_commandsToSend.push_back(command.getFrame());
}

void CanBusStream::requestNodeGroupStatus(uint8_t moduleAddress, uint16_t nodeGroupIndex, bool monitor)
{
    if (m_sending)
        return;

    m_waitForReply = false;

    m_toModuleAddress = moduleAddress;

    CanCommands command;
    command.moduleAddress = moduleAddress;
    command.nodeGroupIndex = nodeGroupIndex;
    command.command = CanCommands::Command::RequestStatus;
    command.data[0] = monitor;
    m_commandsToSend.push_back(command.getFrame());
}

void CanBusStream::changeMode(uint8_t moduleAddress, EBrytecApp::Mode mode)
{
    if (m_sending)
        return;

    m_waitForReply = false;

    m_toModuleAddress = moduleAddress;

    CanCommands command;
    command.moduleAddress = moduleAddress;
    command.command = CanCommands::Command::ChangeMode;
    command.data[0] = mode;
    m_commandsToSend.push_back(command.getFrame());
}

void CanBusStream::changeAddress(uint8_t moduleAddress, uint8_t newAddress)
{
    if (m_sending)
        return;

    m_waitForReply = true;

    m_toModuleAddress = moduleAddress;

    CanCommands command;
    command.moduleAddress = moduleAddress;
    command.command = CanCommands::Command::ChangeAddress;
    command.data[0] = newAddress;
    m_commandsToSend.push_back(command.getFrame());
}

void CanBusStream::reloadConfig(uint8_t moduleAddress)
{
    if (m_sending)
        return;

    m_waitForReply = false;

    m_toModuleAddress = moduleAddress;

    changeMode(moduleAddress, EBrytecApp::Mode::Stopped);

    CanCommands command;
    command.moduleAddress = moduleAddress;
    command.command = CanCommands::Command::ReloadConfig;
    m_commandsToSend.push_back(command.getFrame());
}

void CanBusStream::reserveConfigSize(uint8_t moduleAddress, uint16_t size)
{
    if (m_sending)
        return;

    m_waitForReply = true;

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

    m_waitForReply = true;

    m_toModuleAddress = moduleAddress;

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
}

void CanBusStream::getModuleData(uint8_t moduleAddress, std::function<void(const std::vector<uint8_t>&)> completeCallback,
    bool fullConfig)
{
    if (m_sending)
        return;

    m_waitForReply = true;

    m_toModuleAddress = moduleAddress;

    m_moduleDataBuffer.clear();
    m_moduleDataCallback = completeCallback;

    CanCommands command;
    command.moduleAddress = moduleAddress;
    command.command = CanCommands::Command::RequestDataSize;

    BinaryBufferSerializer ser(command.data, 8);
    ser.writeRaw<bool>(fullConfig);

    m_commandsToSend.push_back(command.getFrame());
}

void CanBusStream::send(std::function<void(CanBusStreamCallbackData)> callback)
{
    if (m_sending)
        return;

    if (m_commandsToSend.size() <= 0)
        return;

    m_callbackData.total = m_commandsToSend.size();
    m_callbackData.leftToSend = m_callbackData.total;
    m_callbackData.error = false;
    m_callback = callback;
    m_sending = true;

    m_timer = 0.0f;
    m_retires = 0;

    sendNextFrame();
}

void CanBusStream::canBusReceived(CanFrame frame)
{
    if (frame.isPinBroadcast()) {

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

    if (frame.isModuleBroadcast()) {
        ModuleStatusBroadcast bc(frame);

        auto it = std::find_if(m_moduleStatuses.begin(), m_moduleStatuses.end(), [bc](ModuleStatusBroadcast& ms) {
            return (bc.moduleAddress == ms.moduleAddress);
        });

        if (it != m_moduleStatuses.end())
            *it = bc;
        else
            m_moduleStatuses.push_back(bc);

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
            sendNextFrame();
        break;

    case CanCommands::Command::RequestDataSize: {
        if (!isCorrectModule)
            return;
        if (m_commandsToSend.size() > 0)
            m_commandsToSend.pop_front();

        BinaryArrayDeserializer des(command.data, 8);
        uint32_t size = 0;
        des.readRaw<uint32_t>(&size);
        bool fullConfig = false;
        des.readRaw<bool>(&fullConfig);

        m_moduleDataBuffer.reserve(size);

        if (size > 0) {

            CanCommands requestDataCommand;
            requestDataCommand.command = CanCommands::RequestData;
            requestDataCommand.moduleAddress = m_toModuleAddress;
            // Copy if full config or not flag
            for (uint32_t i = 0; i < ((size / 8) + 1); i++) {

                BinaryBufferSerializer ser(requestDataCommand.data, 8);
                ser.writeRaw<uint32_t>(i * 8);
                ser.writeRaw<bool>(fullConfig);

                m_commandsToSend.push_back(requestDataCommand.getFrame());
            }
            m_callbackData.total = m_commandsToSend.size();
        }

        if (m_commandsToSend.size() > 0)
            sendNextFrame();

        break;
    }

    case CanCommands::Command::RequestData:
        if (!isCorrectModule)
            return;
        if (m_commandsToSend.size() > 0)
            m_commandsToSend.pop_front();

        m_moduleDataBuffer.insert(m_moduleDataBuffer.end(), std::begin(command.data), std::end(command.data));

        if (m_commandsToSend.size() > 0)
            sendNextFrame();
        else
            m_moduleDataCallback(m_moduleDataBuffer);

        break;

    case CanCommands::Command::RequestAddress:
        return;

    default:
        std::cout << "Unknow can command" << std::endl;
        return;
    }

    m_timer = 0.0f;
    m_retires = 0;

    if (m_commandsToSend.size() == 0)
        m_sending = false;

    m_callbackData.leftToSend = m_commandsToSend.size();
    m_callback(m_callbackData);
}

void CanBusStream::sendNextFrame()
{
    if (m_commandsToSend.size() > 0) {
        m_sendFunction(m_commandsToSend.front());
        if (!m_waitForReply)
            m_commandsToSend.pop_front();
    } else
        std::cout << "Trying to send a can message from an empty queue." << std::endl;
}
}