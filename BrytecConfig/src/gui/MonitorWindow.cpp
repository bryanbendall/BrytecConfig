#include "MonitorWindow.h"

#include "AppManager.h"
#include "data/InternalPin.h"
#include "data/PhysicalPin.h"
#include <IconsFontAwesome5.h>
#include <imgui.h>

namespace Brytec {

MonitorWindow::MonitorWindow()
{
}

void MonitorWindow::drawWindow()
{
    if (!m_opened)
        return;

    float bigColumn = ImGui::CalcTextSize("#####################").x;
    float smallColumn = ImGui::CalcTextSize("#######").x;
    float cellPadding = ImGui::GetStyle().CellPadding.x;

    ImGui::SetNextWindowSize(ImVec2(500.0f, 500.0f), ImGuiCond_FirstUseEver);
    ImGui::Begin(ICON_FA_CHART_BAR " Monitor", &m_opened, ImGuiWindowFlags_HorizontalScrollbar);

    static ImGuiTableFlags flags = ImGuiTableFlags_Sortable | ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter
        | ImGuiTableFlags_BordersV | ImGuiTableFlags_NoBordersInBody | ImGuiTableFlags_ScrollY;

    float tableWidth = bigColumn + (smallColumn * 4) + (cellPadding * 8);
    float columnWidth = tableWidth + ImGui::GetStyle().WindowPadding.x + ImGui::GetStyle().ItemSpacing.x;

    bool isCommunicationOpen = AppManager::getUsbManager().isOpen() || AppManager::getNetManager().isOpen();

    // Module Buttons
    ImGui::BeginDisabled(!isCommunicationOpen);
    if (ImGui::Button("Update##Modules")) {
        AppManager::getCanBusStream().getModuleStatuses().clear();
        AppManager::getCanBusStream().requestModuleStatus(CanCommands::AllModules);
        AppManager::getCanBusStream().send([](CanBusStreamCallbackData data) {});
    }
    ImGui::EndDisabled();

    ImGui::SameLine(columnWidth);

    // Node Group Buttons
    if (ImGui::Button("Clear##Node Groups"))
        AppManager::getCanBusStream().getNodeGroupStatuses().clear();

    ImGui::SameLine();

    std::shared_ptr<Pin> pin = AppManager::getSelected<Pin>();
    std::shared_ptr<NodeGroup> nodeGroup = nullptr;
    if (pin)
        nodeGroup = pin->getNodeGroup();

    ImGui::BeginDisabled(!isCommunicationOpen || !nodeGroup);
    if (ImGui::Button("Monitor Status")) {
        if (nodeGroup) {
            auto moduleAddr = AppManager::getConfig()->getAssignedModuleAddress(nodeGroup);
            auto pinAddr = AppManager::getConfig()->getAssignedPinAddress(nodeGroup);
            AppManager::getCanBusStream().requestNodeGroupStatus(moduleAddr, pinAddr, true);
            AppManager::getCanBusStream().send([](CanBusStreamCallbackData data) {});
        }
    }
    ImGui::EndDisabled();

    ImGui::SameLine();

    bool onBus = nodeGroup ? AppManager::getConfig()->getUsedOnBus(nodeGroup) : false;
    ImGui::BeginDisabled(!isCommunicationOpen || !nodeGroup || onBus);
    if (ImGui::Button("Un-monitor Status")) {
        if (nodeGroup) {
            auto moduleAddr = AppManager::getConfig()->getAssignedModuleAddress(nodeGroup);
            auto pinAddr = AppManager::getConfig()->getAssignedPinAddress(nodeGroup);
            AppManager::getCanBusStream().requestNodeGroupStatus(moduleAddr, pinAddr, false);
            AppManager::getCanBusStream().send([](CanBusStreamCallbackData data) {});
        }
    }
    ImGui::EndDisabled();

    // Headers
    ImGui::Text(ICON_FA_HDD " Modules");
    ImGui::SameLine(columnWidth);
    ImGui::Text(ICON_FA_DICE_D6 " Node Groups");

    if (ImGui::BeginTable("Modules", 5, flags, ImVec2(tableWidth, 0.0f))) {

        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_WidthFixed, bigColumn);
        ImGui::TableSetupColumn("Address", ImGuiTableColumnFlags_WidthFixed, smallColumn);
        ImGui::TableSetupColumn("Node Array", ImGuiTableColumnFlags_WidthFixed, smallColumn);
        ImGui::TableSetupColumn("Loaded", ImGuiTableColumnFlags_WidthFixed, smallColumn);
        ImGui::TableSetupColumn("Mode", ImGuiTableColumnFlags_PreferSortDescending | ImGuiTableColumnFlags_WidthStretch, smallColumn);
        ImGui::TableSetupScrollFreeze(0, 1); // Make row always visible
        ImGui::TableHeadersRow();

        std::vector<ModuleStatusBroadcast>& map = AppManager::getCanBusStream().getModuleStatuses();

        auto sortSpec = ImGui::TableGetSortSpecs();
        bool decending = sortSpec->Specs->SortDirection == ImGuiSortDirection_Descending;
        switch (sortSpec->Specs->ColumnIndex) {
        case 0: // name
            std::sort(map.begin(), map.end(), [decending](ModuleStatusBroadcast a, ModuleStatusBroadcast b) {
                if (decending)
                    return a.moduleAddress > b.moduleAddress;
                else
                    return a.moduleAddress < b.moduleAddress;
            });
            break;
        case 1: // address
            std::sort(map.begin(), map.end(), [decending](ModuleStatusBroadcast a, ModuleStatusBroadcast b) {
                if (decending)
                    return a.moduleAddress > b.moduleAddress;
                else
                    return a.moduleAddress < b.moduleAddress;
            });
            break;
        case 2: // loaded
            std::sort(map.begin(), map.end(), [decending](ModuleStatusBroadcast a, ModuleStatusBroadcast b) {
                if (decending)
                    return a.deserializeOk > b.deserializeOk;
                else
                    return a.deserializeOk < b.deserializeOk;
            });
            break;
        case 3: // mode
            std::sort(map.begin(), map.end(), [decending](ModuleStatusBroadcast a, ModuleStatusBroadcast b) {
                if (decending)
                    return a.mode > b.mode;
                else
                    return a.mode < b.mode;
            });
            break;
        }

        for (auto& ms : AppManager::getCanBusStream().getModuleStatuses()) {
            std::string modeString;
            switch (ms.mode) {
            case EBrytecApp::Mode::Normal:
                modeString = "Normal";
                break;
            case EBrytecApp::Mode::Stopped:
                modeString = "Stopped";
                break;
            }
            auto module = AppManager::getConfig()->findModule(ms.moduleAddress);

            ImGui::PushID(ms.moduleAddress);
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            if (module)
                ImGui::Text("%s", module->getName().c_str());
            else
                ImGui::Text("Module: %d", ms.moduleAddress);
            ImGui::TableNextColumn();
            ImGui::Text("%d", ms.moduleAddress);
            ImGui::TableNextColumn();
            ImGui::Text("%d%%", ms.nodePercent);
            ImGui::TableNextColumn();
            ImGui::Text("%s", ms.deserializeOk ? "Yes" : "No");
            ImGui::TableNextColumn();
            ImGui::Text("%s", modeString.c_str());
            ImGui::PopID();
        }

        ImGui::EndTable();
    }

    ImGui::SameLine();

    tableWidth = (bigColumn * 2) + (smallColumn * 4) + (cellPadding * 12);
    if (ImGui::BeginTable("Node Groups", 6, flags, ImVec2(tableWidth, 0.0f))) {

        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_WidthFixed, bigColumn);
        ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthFixed, smallColumn);
        ImGui::TableSetupColumn("Status", ImGuiTableColumnFlags_WidthFixed, smallColumn);
        ImGui::TableSetupColumn("Current", ImGuiTableColumnFlags_WidthFixed, smallColumn);
        ImGui::TableSetupColumn("Voltage", ImGuiTableColumnFlags_WidthFixed, smallColumn);
        ImGui::TableSetupColumn("Module", ImGuiTableColumnFlags_PreferSortDescending | ImGuiTableColumnFlags_WidthStretch, 0.0f);
        ImGui::TableSetupScrollFreeze(0, 1); // Make row always visible
        ImGui::TableHeadersRow();

        {
            // Sort
            std::vector<PinStatusBroadcast>& vec = AppManager::getCanBusStream().getNodeGroupStatuses();

            ImGuiTableSortSpecs* sortSpec = ImGui::TableGetSortSpecs();
            bool decending = sortSpec->Specs->SortDirection == ImGuiSortDirection_Descending;
            switch (sortSpec->Specs->ColumnIndex) {
            case 0: // name
                std::sort(vec.begin(), vec.end(), [decending](PinStatusBroadcast a, PinStatusBroadcast b) {
                    if (decending)
                        return a.nodeGroupIndex > b.nodeGroupIndex;
                    else
                        return a.nodeGroupIndex < b.nodeGroupIndex;
                });
                break;
            case 1: // value
                std::sort(vec.begin(), vec.end(), [decending](PinStatusBroadcast a, PinStatusBroadcast b) {
                    if (decending)
                        return a.value > b.value;
                    else
                        return a.value < b.value;
                });
                break;
            case 2: // statusFlags
                std::sort(vec.begin(), vec.end(), [decending](PinStatusBroadcast a, PinStatusBroadcast b) {
                    if (decending)
                        return a.statusFlags > b.statusFlags;
                    else
                        return a.statusFlags < b.statusFlags;
                });
                break;
            case 3: // current
                std::sort(vec.begin(), vec.end(), [decending](PinStatusBroadcast a, PinStatusBroadcast b) {
                    if (decending)
                        return a.current > b.current;
                    else
                        return a.current < b.current;
                });
                break;
            case 4: // voltage
                std::sort(vec.begin(), vec.end(), [decending](PinStatusBroadcast a, PinStatusBroadcast b) {
                    if (decending)
                        return a.voltage > b.voltage;
                    else
                        return a.voltage < b.voltage;
                });
                break;
            case 5: // module
                std::sort(vec.begin(), vec.end(), [decending](PinStatusBroadcast a, PinStatusBroadcast b) {
                    if (decending)
                        return a.moduleAddress > b.moduleAddress;
                    else
                        return a.moduleAddress < b.moduleAddress;
                });
                break;
            }
        }

        for (auto& pinStatus : AppManager::getCanBusStream().getNodeGroupStatuses()) {

            std::shared_ptr<Module> module = AppManager::getConfig()->findModule(pinStatus.moduleAddress);
            std::shared_ptr<NodeGroup> nodeGroup = nullptr;

            if (module) {
                if (pinStatus.nodeGroupIndex < module->getPhysicalPins().size())
                    nodeGroup = module->getPhysicalPins()[pinStatus.nodeGroupIndex]->getNodeGroup();
                else if (pinStatus.nodeGroupIndex < module->getPhysicalPins().size() + module->getInternalPins().size())
                    nodeGroup = module->getInternalPins()[pinStatus.nodeGroupIndex - module->getPhysicalPins().size()]->getNodeGroup();
            }

            ImGui::PushID(nodeGroup.get());
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            if (nodeGroup)
                ImGui::Text("%s", nodeGroup->getName().c_str());
            else
                ImGui::Text("Module: %d Node Group: %d", pinStatus.moduleAddress, pinStatus.nodeGroupIndex);
            ImGui::TableNextColumn();
            ImGui::Text("%.2f", pinStatus.value);
            ImGui::TableNextColumn();
            ImGui::TextUnformatted("Normal");
            ImGui::TableNextColumn();
            ImGui::Text("%.2f A", pinStatus.current);
            ImGui::TableNextColumn();
            ImGui::Text("%.2f V", pinStatus.voltage);
            ImGui::TableNextColumn();
            if (module)
                ImGui::Text("%s", module->getName().c_str());
            else
                ImGui::Text("Module: %d", pinStatus.moduleAddress);
            ImGui::PopID();
        }

        ImGui::EndTable();
    }

    ImGui::End();
}
}