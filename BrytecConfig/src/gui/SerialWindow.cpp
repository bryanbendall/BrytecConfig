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

        ImGui::SameLine();

        if (ImGui::Button("Set Stopped")) {
            AppManager::getCanBusStream().changeMode(module->getAddress(), EBrytecApp::Mode::Stopped);
            AppManager::getCanBusStream().send(sendingCallback);
        }

        ImGui::SameLine();

        if (ImGui::Button("Set Normal")) {
            AppManager::getCanBusStream().changeMode(module->getAddress(), EBrytecApp::Mode::Normal);
            AppManager::getCanBusStream().send(sendingCallback);
        }

        ImGui::SameLine();

        if (ImGui::Button("Reload")) {
            AppManager::getCanBusStream().reloadConfig(module->getAddress());
            AppManager::getCanBusStream().send(sendingCallback);
        }
        ImGui::EndDisabled();

        ImGui::ProgressBar((float)(s_canData.total - s_canData.leftToSend) / (float)s_canData.total);

        if (s_canData.error)
            ImGui::TextUnformatted("Sending error!");
        else
            ImGui::Text("Sending %u out of %u", s_canData.leftToSend, s_canData.total);

        if (ImGui::Button("Get Module Statuses")) {
            AppManager::getCanBusStream().requestModuleStatus(CanCommands::AllModules);
            AppManager::getCanBusStream().send(sendingCallback);
        }

        ImGui::SameLine();

        std::shared_ptr<Pin> pin = AppManager::getSelected<Pin>();
        std::shared_ptr<NodeGroup> nodeGroup = nullptr;
        if (pin)
            nodeGroup = pin->getNodeGroup();

        ImGui::BeginDisabled(AppManager::getCanBusStream().isSending() || !nodeGroup);
        if (ImGui::Button("Get Node Group Status")) {

            if (nodeGroup) {
                auto moduleAddr = AppManager::getConfig()->getAssignedModuleAddress(nodeGroup);
                auto pinAddr = AppManager::getConfig()->getAssignedPinAddress(nodeGroup);
                AppManager::getCanBusStream().requestNodeGroupStatus(moduleAddr, pinAddr);
                AppManager::getCanBusStream().send(sendingCallback);
            }
        }
        ImGui::EndDisabled();

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

    ImGui::Separator();
    ImGui::TextUnformatted("Module Statuses");
    for (auto [addr, ms] : AppManager::getCanBusStream().getModuleStatuses()) {
        std::string modeString;
        switch (ms.mode) {
        case EBrytecApp::Mode::Normal:
            modeString = "Normal";
            break;
        case EBrytecApp::Mode::Programming:
            modeString = "Programming";
            break;
        case EBrytecApp::Mode::Stopped:
            modeString = "Stopped";
            break;
        }
        auto module = AppManager::getConfig()->findModule(ms.address);
        ImGui::Indent();
        ImGui::Text("Module Address: %s", module ? module->getName().c_str() : "Unknown");
        ImGui::Indent();
        ImGui::Text("Address: %d", ms.address);
        ImGui::Text("Mode: %s", modeString.c_str());
        ImGui::Text("Deserialized: %s", ms.deserializeOk ? "true" : "false");
        ImGui::Unindent();
        ImGui::Unindent();
    }
    ImGui::Separator();
    ImGui::TextUnformatted("Node Group Statuses");
    for (auto& pinStatus : AppManager::getCanBusStream().getNodeGroupStatuses()) {

        if (auto module = AppManager::getConfig()->findModule(pinStatus.moduleAddress)) {
            std::shared_ptr<NodeGroup> nodeGroup = nullptr;
            if (pinStatus.nodeGroupIndex >= module->getPhysicalPins().size())
                nodeGroup = module->getInternalPins()[pinStatus.nodeGroupIndex - module->getPhysicalPins().size()]->getNodeGroup();
            else
                nodeGroup = module->getPhysicalPins()[pinStatus.nodeGroupIndex]->getNodeGroup();

            ImGui::Indent();
            if (nodeGroup)
                ImGui::Text("%s", nodeGroup->getName().c_str());
            else
                ImGui::Text("Module Addr: %d Node Group Index: %d", pinStatus.moduleAddress, pinStatus.nodeGroupIndex);
            ImGui::Indent();
            // ImGui::Text("Current: %f", pinStatus.current);
            // ImGui::Text("Voltage: %f", pinStatus.voltage);
            ImGui::Text("Value: %f", pinStatus.value);
            ImGui::Unindent();
            ImGui::Unindent();
        }
    }

    ImGui::End();
}

}