#include "ConnectionHandler.h"

#include "BrytecConfigEmbedded/EBrytecApp.h"
#include <iostream>

static Brytec::UsbPacket getPacket(std::vector<uint8_t>& data)
{
    Brytec::UsbPacket packet;

    if (data.size() <= 0)
        return packet;

    auto start = std::find(data.begin(), data.end(), Brytec::PacketStart);
    if (start > data.begin()) {
        data.erase(data.begin(), start);
    }

    if (data.size() >= 2)
        packet.length = data[1];
    else
        return Brytec::UsbPacket {};

    if (packet.length > 64) {
        // Something wrong with packet, delete it and try again
        std::cout << "Discarding data ***********************************" << std::endl;
        data.erase(data.begin());
        return Brytec::UsbPacket {};
    }

    if (data.size() >= packet.length + 2)
        memcpy(packet.data, &data[2], packet.length);
    else
        return Brytec::UsbPacket {};

    data.erase(data.begin(), data.begin() + packet.length + 2);
    return packet;
}

ConnectionHandler::ConnectionHandler(asio::io_service& io_service)
    : sock(io_service)
{
}

std::shared_ptr<ConnectionHandler> ConnectionHandler::create(asio::io_service& io_service)
{
    return std::shared_ptr<ConnectionHandler>(new ConnectionHandler(io_service));
}

tcp::socket& ConnectionHandler::socket()
{
    return sock;
}

void ConnectionHandler::start()
{
    startRead();
}

void ConnectionHandler::send(const Brytec::UsbPacket& packet)
{
    m_txMutex.lock();

    m_txPackets.push_back(packet);

    if (m_txPackets.size() == 1)
        sendBytes();

    m_txMutex.unlock();
}

void ConnectionHandler::handle_read(const asio::error_code& err, size_t bytes_transferred)
{
    if (!err) {

        if (bytes_transferred > 0) {
            m_readData.insert(m_readData.end(), std::begin(data), std::begin(data) + bytes_transferred);

            while (Brytec::UsbPacket packet = getPacket(m_readData)) {
                m_receiveCallback(packet);
            }
        }

        startRead();

    } else {
        std::cerr << "error: " << err.message() << std::endl;
        sock.close();
    }
}
void ConnectionHandler::handle_write(const asio::error_code& err, size_t bytes_transferred)
{
    if (!err) {

        m_txMutex.lock();

        if (m_txPackets.size() > 0)
            sendBytes();

        m_txMutex.unlock();

    } else {
        std::cerr << "error: " << err.message() << std::endl;
        sock.close();
    }
}

void ConnectionHandler::startRead()
{
    sock.async_read_some(
        asio::buffer(data, 1024),
        std::bind(&ConnectionHandler::handle_read,
            shared_from_this(),
            asio::placeholders::error,
            asio::placeholders::bytes_transferred));
}

void ConnectionHandler::startWrite()
{
    sock.async_write_some(
        asio::buffer(m_writeBuffer, m_writeBuffer[1] + 2),
        std::bind(&ConnectionHandler::handle_write,
            shared_from_this(),
            asio::placeholders::error,
            asio::placeholders::bytes_transferred));
}

void ConnectionHandler::sendBytes()
{
    if (m_txPackets.size() <= 0) {
        std::cout << "Trying to send a packet from empty queue" << std::endl;
        return;
    }

    m_writeBuffer[0] = Brytec::PacketStart;
    m_writeBuffer[1] = m_txPackets[0].length;
    memcpy(&m_writeBuffer[2], m_txPackets[0].data, m_txPackets[0].length);

    m_txPackets.pop_front();

    startWrite();
}