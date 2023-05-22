#pragma once

#include "Usb.h"
#include "communication/CanBusStream.h"
#include <deque>

namespace Brytec {

class UsbManager {

public:
    UsbManager();
    ~UsbManager();
    void update();

    std::vector<serial::PortInfo> getAvailablePorts() { return serial::list_ports(); }
    const serial::PortInfo& getDevice() { return m_device; }
    void setDevice(const serial::PortInfo& device) { m_device = device; }

    void open();
    void close() { m_usb.close(); }
    bool isOpen() { return m_usb.isOpen(); }

    void send(CanExtFrame& frame);
    void onReceive(UsbPacket packet);

private:
    Usb m_usb;
    serial::PortInfo m_device;
    CanBusStream m_stream;

    std::function<void(CanExtFrame)> m_canBusCallback;
};
}