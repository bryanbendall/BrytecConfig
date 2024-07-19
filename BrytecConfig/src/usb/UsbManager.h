#pragma once

#include "Usb.h"
#include "Usb/ModuleCommand.h"
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
    bool isDeviceValid();

    void open();
    void close() { m_usb.close(); }
    bool isOpen() { return m_usb.isOpen(); }

    void sendCan(CanFrame& frame);
    void sendCommand(ModuleCommand moduleCommand);
    void onReceive(UsbPacket packet);

private:
    Usb m_usb;
    serial::PortInfo m_device;

    std::function<void(CanFrame)> m_canBusCallback;
};
}