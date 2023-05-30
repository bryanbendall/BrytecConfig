#include "SerialWindow.h"

#include "AppManager.h"
#include "BrytecConfigEmbedded/Deserializer/BinaryArrayDeserializer.h"
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

        static uint8_t newAddress = 0;
        static bool showButtons = true;
        ImGui::InputScalar("###ModuleAddressInput", ImGuiDataType_U8, &newAddress, &showButtons);

        ImGui::SameLine();

        if (ImGui::Button("Change Address")) {
            AppManager::getCanBusStream().changeAddress(module->getAddress(), newAddress);
            AppManager::getCanBusStream().send(sendingCallback);
        }

        ImGui::EndDisabled();

        if (ImGui::Button("Add module template")) {
            AppManager::getCanBusStream().getModuleData(
                newAddress,
                [](const std::vector<uint8_t>& buffer) {
                    std::shared_ptr<Module> module = std::make_shared<Module>();
                    ModuleSerializer serializer(module);
                    BinaryArrayDeserializer des(buffer.data(), buffer.size());
                    if (serializer.deserializeTemplateBinary(des)) {
                        AppManager::getConfig()->addModule(module);
                    }
                },
                false);
            AppManager::getCanBusStream().send(sendingCallback);
        }

        ImGui::SameLine();

        if (ImGui::Button("Add full module")) {
            AppManager::getCanBusStream().getModuleData(
                newAddress,
                [](const std::vector<uint8_t>& buffer) {
                    std::shared_ptr<Module> module = AppManager::getConfig()->findModule(newAddress);
                    if (module) {
                        ModuleSerializer serializer(AppManager::getConfig(), module);
                        BinaryArrayDeserializer des(buffer.data(), buffer.size());
                        serializer.deserializeBinary(des);
                    } else {
                        std::cout << "Need to get module template first" << std::endl;
                    }
                },
                true);
            AppManager::getCanBusStream().send(sendingCallback);
        }

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

    ImGui::End();
}

}