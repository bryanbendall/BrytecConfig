#pragma once

#include "BrytecConfigEmbedded/Can/EBrytecCan.h"
#include "BrytecConfigEmbedded/EBrytecApp.h"
#include "usb/Usb.h"
#include <atomic>
#include <deque>
#include <functional>
#include <mutex>
#include <thread>
#include <vector>

namespace Brytec {

struct CanBusStreamCallbackData {
    uint16_t total = 0;
    uint16_t leftToSend = 0;
    bool error = false;
};

class CanBusStream {

public:
    CanBusStream() = default;

    void changeMode(uint8_t moduleAddress, EBrytecApp::Mode mode);
    void reloadConfig(uint8_t moduleAddress);
    void reserveConfigSize(uint8_t moduleAddress, uint16_t size);
    void sendNewConfig(uint8_t moduleAddress, std::vector<uint8_t>& data);

    void setSendFunction(std::function<void(CanExtFrame&)> function) { m_sendFunction = function; }
    void send(std::function<void(CanBusStreamCallbackData)> callback);
    bool isSending() { return m_sending; }

    void canBusReceived(CanExtFrame frame);

private:
    std::deque<CanExtFrame> m_commandsToSend;
    std::function<void(CanBusStreamCallbackData)> m_callback;
    CanBusStreamCallbackData m_callbackData;
    std::atomic_bool m_sending = false;
    std::function<void(CanExtFrame&)> m_sendFunction;
};

}