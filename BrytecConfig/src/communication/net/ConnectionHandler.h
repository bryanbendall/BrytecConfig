#pragma once

#include "BrytecConfigEmbedded/Usb/UsbDefs.h"
#include <asio.hpp>
#include <deque>
#include <memory>
#include <mutex>

using asio::ip::tcp;

class ConnectionHandler : public std::enable_shared_from_this<ConnectionHandler> {

public:
    ConnectionHandler(asio::io_service& io_service);
    static std::shared_ptr<ConnectionHandler> create(asio::io_service& io_service);
    tcp::socket& socket();
    void start();
    void send(const Brytec::UsbPacket& packet);

    void handle_read(const asio::error_code& err, size_t bytes_transferred);
    void handle_write(const asio::error_code& err, size_t bytes_transferred);

    void setReceiveCallback(std::function<void(Brytec::UsbPacket)> callback) { m_receiveCallback = callback; }

private:
    void startRead();
    void startWrite();
    void sendBytes();

private:
    tcp::socket sock;
    char data[1024];
    std::vector<uint8_t> m_readData;
    uint8_t m_writeBuffer[64];
    std::mutex m_txMutex;
    std::deque<Brytec::UsbPacket> m_txPackets;
    std::function<void(Brytec::UsbPacket)> m_receiveCallback;
};