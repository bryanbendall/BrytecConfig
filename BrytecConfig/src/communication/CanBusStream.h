#pragma once

#include "BrytecConfigEmbedded/Can/EBrytecCan.h"
#include "BrytecConfigEmbedded/EBrytecApp.h"
#include <atomic>
#include <deque>
#include <functional>

namespace Brytec {

struct CanBusStreamCallbackData {
    uint16_t total = 0;
    uint16_t leftToSend = 0;
    bool error = false;
};

class CanBusStream {

public:
    CanBusStream() = default;

    void update();

    void requestModuleStatus(uint8_t moduleAddress);
    void requestNodeGroupStatus(uint8_t moduleAddress, uint16_t nodeGroupIndex, bool monitor);
    void changeMode(uint8_t moduleAddress, EBrytecApp::Mode mode);
    void changeAddress(uint8_t moduleAddress, uint8_t newAddress);
    void reloadConfig(uint8_t moduleAddress);
    void reserveConfigSize(uint8_t moduleAddress, uint16_t size);
    void sendNewConfig(uint8_t moduleAddress, std::vector<uint8_t>& data);
    void getModuleData(uint8_t moduleAddress, std::function<void(const std::vector<uint8_t>&)> completeCallback, bool fullConfig);

    void setSendFunction(std::function<void(CanFrame&)> function) { m_sendFunction = function; }
    void send(std::function<void(CanBusStreamCallbackData)> callback);
    bool isSending() { return m_sending; }

    std::vector<ModuleStatusBroadcast>& getModuleStatuses() { return m_moduleStatuses; }
    std::vector<PinStatusBroadcast>& getNodeGroupStatuses() { return m_nodeGroupStatuses; }

    void canBusReceived(CanFrame frame);

private:
    void sendNextFrame();

private:
    float m_timer = 0.0f;

    std::deque<CanFrame> m_commandsToSend;
    std::function<void(CanBusStreamCallbackData)> m_callback;
    CanBusStreamCallbackData m_callbackData;
    std::atomic_bool m_sending = false;
    uint8_t m_toModuleAddress;
    std::function<void(CanFrame&)> m_sendFunction;
    std::atomic_bool m_waitForReply = true;

    std::vector<uint8_t> m_moduleDataBuffer;
    std::function<void(const std::vector<uint8_t>&)> m_moduleDataCallback;

    uint8_t m_retires = 0;
    static constexpr uint8_t s_maxRetries = 5;

    std::vector<ModuleStatusBroadcast> m_moduleStatuses;
    std::vector<PinStatusBroadcast> m_nodeGroupStatuses;
};

}