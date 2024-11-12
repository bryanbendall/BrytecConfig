#include "Client.h"

#include <chrono>
#include <iostream>

using namespace std::chrono_literals;

namespace Brytec {

Client::Client()
{
}

void Client::startConnect(std::string ipAddress, uint32_t port)
{
    if (!m_runThread && m_thread.joinable())
        m_thread.join();

    m_thread = std::thread([this, ipAddress, port]() {
        std::shared_ptr<ConnectionHandler> connection = ConnectionHandler::create(m_io_service);
        m_connection = connection;
        connection->setReceiveCallback(m_receiveCallback);

        try {
            std::cout << "Trying to connect to: " << ipAddress << " Port: " << port << std::endl;
            connection->socket().connect(tcp::endpoint(asio::ip::address::from_string(ipAddress), port));
        } catch (asio::system_error& e) {
            std::cout << "Client error: " << e.what() << std::endl;
            return;
        }

        // We are connected here
        connection->start();

        m_runThread = true;
        while (m_runThread) {
            m_io_service.poll();

            if (!isConnected())
                m_runThread = false;

            std::this_thread::sleep_for(1ms);
        }
    });
}

void Client::close()
{
    if (auto connection = m_connection.lock())
        connection->socket().close();
    m_connection.reset();
    m_io_service.stop();
    m_runThread = false;
    if (m_thread.joinable())
        m_thread.join();
}

bool Client::isConnected()
{
    auto connection = m_connection.lock();
    return (bool)connection;
}

void Client::send(const Brytec::UsbPacket& packet)
{
    if (auto connection = m_connection.lock())
        connection->send(packet);
}

}