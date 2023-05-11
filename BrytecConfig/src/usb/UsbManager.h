#pragma once

#include "Usb.h"
#include <deque>

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

    bool getDoneSendingConfig() { return m_doneSendingConfig; }
    void sendConfig(uint8_t moduleAddress, std::vector<uint8_t>& data);
    uint32_t getAmountToSend() { return m_amountToSend; }
    uint32_t getAmountLeftToSend() { return m_amountLeftToSend; }

private:
    Usb m_usb;
    serial::PortInfo m_device;
    std::vector<CanExtFrame> m_canFrames;

    bool m_doneSendingConfig = true;
    bool m_waitForReturn = false;
    std::deque<CanExtFrame> m_canToSend;
    uint32_t m_amountToSend = 0;
    uint32_t m_amountLeftToSend = 0;
};
}