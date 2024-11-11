#pragma once

#include "Client.h"
#include "Usb/ModuleCommand.h"
#include "communication/CanBusStream.h"

namespace Brytec {

class NetManager {

public:
    NetManager();
    ~NetManager();
    void update();

    void open();
    void close() { m_client.close(); }
    bool isOpen() { return m_client.isConnected(); }

    void sendCan(CanFrame& frame);
    void sendCommand(ModuleCommand moduleCommand);
    void onReceive(UsbPacket packet);

private:
    std::function<void(CanFrame)> m_canBusCallback;
    Client m_client;
};
}