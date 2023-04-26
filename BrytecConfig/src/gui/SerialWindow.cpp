#include "SerialWindow.h"

#include "imgui.h"
#include <string>

namespace Brytec {

void SerialWindow::drawWindow()
{
    if (!m_opened)
        return;

    ImGui::Begin("Serial", &m_opened);

    if (m_usb.isOpen()) {

        ImGui::Text("Connected to: %s", m_selectedDevice.description.c_str());

        if (ImGui::Button("close serial"))
            m_usb.close();

        ImGui::SameLine();

        if (ImGui::Button("clear data")) {
            m_canMap.clear();
            m_canFrames.clear();
        }

        ImGui::SameLine();

        ImGui::Text("Total Messages: %d", m_canFrames.size());

        ImGui::SameLine();

        if (ImGui::Button("send packet")) {
            static int i = 0;
            i++;
            CanExtFrame can;
            can.id = 52;
            can.data[0] = i;
            UsbPacket packet;
            packet.set(can);
            m_usb.send(packet);

            can.id = 66;
            can.data[1] = i;
            packet.set(can);
            m_usb.send(packet);
        }

    } else {

        std::vector<serial::PortInfo> availablePorts = m_usb.getAvailablePorts();

        auto ret = std::find_if(availablePorts.begin(), availablePorts.end(), [this](serial::PortInfo info) { return info.description == m_selectedDevice.description; });
        if (ret == availablePorts.end())
            m_selectedDevice.description = "";

        if (ImGui::BeginCombo("##Serial Ports", m_selectedDevice.description.c_str())) {
            for (auto& device : availablePorts) {
                if (ImGui::Selectable(device.description.c_str(), device.port == m_selectedDevice.port)) {
                    m_selectedDevice = device;
                }
            }
            ImGui::EndCombo();
        }

        if (ImGui::Button("open serial"))
            m_usb.open(m_selectedDevice.port);

        ImGui::SameLine();

        if (ImGui::Button("clear data"))
            m_canFrames.clear();
    }

    ImGui::Separator();

    std::vector<UsbPacket> newPackets = m_usb.getPackets();

    ImGui::Text("%d", newPackets.size());

    for (auto& newPacket : newPackets) {
        if (auto frame = newPacket.as<CanExtFrame>())
            m_canMap[frame.id] = frame;
        // m_canFrames.push_back(frame);
    }

    for (auto& m : m_canMap) {
        auto& f = m.second;
        ImGui::Text("id: %d dlc: %d data: %d %d %d %d %d %d %d %d",
            f.id,
            f.dlc,
            f.data[0],
            f.data[1],
            f.data[2],
            f.data[3],
            f.data[4],
            f.data[5],
            f.data[6],
            f.data[7]);
    }

    ImGui::End();
}

}