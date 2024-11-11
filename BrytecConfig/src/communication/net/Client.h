#pragma once

#include "BrytecConfigEmbedded/Usb/UsbDefs.h"
#include "ConnectionHandler.h"
#include <asio.hpp>

using asio::ip::tcp;

namespace Brytec {

class Client {

public:
    Client();
    void startConnect(std::string ipAddress, uint32_t port);
    void close();
    bool isConnected();
    void send(const UsbPacket& packet);

    void setReceiveCallback(std::function<void(UsbPacket)> callback) { m_receiveCallback = callback; }

private:
    asio::io_service m_io_service;
    static inline std::weak_ptr<ConnectionHandler> m_connection;
    std::function<void(UsbPacket)> m_receiveCallback;
    std::thread m_thread;
    bool m_runThread = false;
};
}