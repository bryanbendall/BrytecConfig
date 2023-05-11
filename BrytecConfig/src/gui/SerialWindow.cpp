#include "SerialWindow.h"

#include "AppManager.h"
#include "BrytecConfigEmbedded/EBrytecApp.h"
#include "imgui.h"
#include "utils/ModuleSerializer.h"
#include <string>

namespace Brytec {

void SerialWindow::drawWindow()
{
    if (!m_opened)
        return;

    UsbManager& usb = AppManager::getUsbManager();

    ImGui::Begin("Serial", &m_opened);

    if (AppManager::getUsbManager().isOpen()) {

        ImGui::Text("Connected to: %s", usb.getDevice().description.c_str());
        ImGui::Text("Hardware id: %s", usb.getDevice().hardware_id.c_str());
        ImGui::Text("Port: %s", usb.getDevice().port.c_str());

        if (ImGui::Button("close serial"))
            usb.close();

        ImGui::SameLine();

        // if (ImGui::Button("clear data")) {
        //     m_canMap.clear();
        // }

        // ImGui::SameLine();

        // if (ImGui::Button("send packet")) {
        //     static int i = 0;
        //     i++;
        //     CanExtFrame can;
        //     can.id = 52;
        //     can.data[0] = i;
        //     usb.send(can);
        // }

        // ImGui::SameLine();

        std::shared_ptr<Module> module = AppManager::getSelected<Module>();
        ImGui::BeginDisabled(!AppManager::getUsbManager().getDoneSendingConfig() || !module);
        if (ImGui::Button("Send Config")) {

            if (module) {
                ModuleSerializer moduleSer(module);
                BinarySerializer ser = moduleSer.serializeBinary();
                AppManager::getUsbManager().sendConfig(module->getAddress(), ser.getData());
            }
        }
        ImGui::EndDisabled();

        ImGui::Separator();

        ImGui::ProgressBar((float)(usb.getAmountToSend() - usb.getAmountLeftToSend()) / (float)usb.getAmountToSend());

        ImGui::Text("Sending %u out of %u", usb.getAmountLeftToSend(), usb.getAmountToSend());

    } else {
        std::vector<serial::PortInfo> availablePorts = AppManager::getUsbManager().getAvailablePorts();
        std::string displayText = usb.getDevice().description;

        auto ret = std::find_if(availablePorts.begin(), availablePorts.end(), [&](serial::PortInfo info) { return info.description == displayText; });
        if (ret == availablePorts.end())
            displayText = "";

        if (ImGui::BeginCombo("##Serial Ports", displayText.c_str())) {
            for (auto& device : availablePorts) {
                if (ImGui::Selectable(device.description.c_str(), device.description == displayText)) {
                    usb.setDevice(device);
                }
            }
            ImGui::EndCombo();
        }

        if (ImGui::Button("open serial"))
            usb.open();

        ImGui::SameLine();

        if (ImGui::Button("clear data")) {
            m_canMap.clear();
        }
    }

    ImGui::Separator();

    for (auto& frame : usb.getCanFrames()) {
        m_canMap[frame.id] = frame;
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