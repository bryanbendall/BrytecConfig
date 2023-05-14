#include "SerialWindow.h"

#include "AppManager.h"
#include "BrytecConfigEmbedded/EBrytecApp.h"
#include "imgui.h"
#include "utils/ModuleSerializer.h"
#include <string>

namespace Brytec {

static CanBusStreamCallbackData s_canData;

static void sendingCallback(CanBusStreamCallbackData data)
{
    s_canData = data;
}

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

        std::shared_ptr<Module> module = AppManager::getSelected<Module>();

        ImGui::BeginDisabled(AppManager::getCanBusStream().isSending() || !module);
        if (ImGui::Button("Send Config")) {

            if (module) {
                ModuleSerializer moduleSer(module);
                BinarySerializer ser = moduleSer.serializeBinary();
                AppManager::getCanBusStream().sendNewConfig(module->getAddress(), ser.getData());
                AppManager::getCanBusStream().send(sendingCallback);
            }
        }
        ImGui::EndDisabled();

        ImGui::ProgressBar((float)(s_canData.total - s_canData.leftToSend) / (float)s_canData.total);

        if (s_canData.error)
            ImGui::TextUnformatted("Sending error!");
        else
            ImGui::Text("Sending %u out of %u", s_canData.leftToSend, s_canData.total);

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
    }

    ImGui::End();
}

}