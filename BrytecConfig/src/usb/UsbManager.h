#pragma once

#include "Usb.h"

namespace Brytec {

class UsbManager {

public:
    UsbManager() = default;
    void update();

    std::vector<serial::PortInfo> getAvailablePorts() { return serial::list_ports(); }
    const serial::PortInfo& getDevice() { return m_device; }
    void setDevice(const serial::PortInfo& device) { m_device = device; }

    void open() { m_usb.open(m_device.port); }
    void close() { m_usb.close(); }
    bool isOpen() { return m_usb.isOpen(); }

    void send(CanExtFrame& frame);
    std::vector<CanExtFrame> getCanFrames();

private:
    Usb m_usb;
    serial::PortInfo m_device;
    std::vector<CanExtFrame> m_canFrames;
};
}