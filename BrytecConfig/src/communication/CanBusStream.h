#pragma once

#include "BrytecConfigEmbedded/Can/EBrytecCan.h"
#include "BrytecConfigEmbedded/EBrytecApp.h"
#include "usb/Usb.h"
#include <atomic>
#include <deque>
#include <functional>
#include <map>

namespace Brytec {

struct CanBusStreamCallbackData {
    uint16_t total = 0;
    uint16_t leftToSend = 0;
    bool error = false;
};

struct ModuleStatus {
    uint8_t address = 0;
    bool deserializeOk = false;
    EBrytecApp::Mode mode = EBrytecApp::Mode::Stopped;
};

class CanBusStream {

public:
    CanBusStream() = default;

    void update();

    void requestModuleStatus(uint8_t moduleAddress);
    void requestNodeGroupStatus(uint8_t moduleAddress, uint16_t nodeGroupIndex);
    void changeMode(uint8_t moduleAddress, EBrytecApp::Mode mode);
    void changeAddress(uint8_t moduleAddress, uint8_t newAddress);
    void reloadConfig(uint8_t moduleAddress);
    void reserveConfigSize(uint8_t moduleAddress, uint16_t size);
    void sendNewConfig(uint8_t moduleAddress, std::vector<uint8_t>& data);

    void setSendFunction(std::function<void(CanExtFrame&)> function) { m_sendFunction = function; }
    void send(std::function<void(CanBusStreamCallbackData)> callback);
    bool isSending() { return m_sending; }

    std::map<uint8_t, ModuleStatus>& getModuleStatuses() { return m_moduleStatuses; }
    std::vector<PinStatusBroadcast>& getNodeGroupStatuses() { return m_nodeGroupStatuses; }

    void canBusReceived(CanExtFrame frame);

private:
    float m_timer = 0.0f;

    std::deque<CanExtFrame> m_commandsToSend;
    std::function<void(CanBusStreamCallbackData)> m_callback;
    CanBusStreamCallbackData m_callbackData;
    std::atomic_bool m_sending = false;
    uint8_t m_toModuleAddress;
    std::function<void(CanExtFrame&)> m_sendFunction;

    uint8_t m_retires = 0;
    static constexpr uint8_t s_maxRetries = 5;

    std::map<uint8_t, ModuleStatus> m_moduleStatuses;
    std::vector<PinStatusBroadcast> m_nodeGroupStatuses;
};

}