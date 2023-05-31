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
    ImGui::Begin(ICON_FA_CHART_BAR " Monitor", &m_opened, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_MenuBar);

    drawMenubar();

    static ImGuiTableFlags flags = ImGuiTableFlags_Sortable | ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter
        | ImGuiTableFlags_BordersV | ImGuiTableFlags_NoBordersInBody | ImGuiTableFlags_ScrollY;

    float tableWidth = bigColumn + (smallColumn * 3) + (cellPadding * 8);

    ImGui::Text(ICON_FA_HDD " Modules");
    ImGui::SameLine(tableWidth + ImGui::GetStyle().WindowPadding.x + ImGui::GetStyle().ItemSpacing.x);
    ImGui::Text(ICON_FA_DICE_D6 " Node Groups");

    if (ImGui::BeginTable("Modules", 4, flags, ImVec2(tableWidth, 0.0f))) {

        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_WidthFixed, bigColumn);
        ImGui::TableSetupColumn("Address", ImGuiTableColumnFlags_WidthFixed, smallColumn);
        ImGui::TableSetupColumn("Loaded", ImGuiTableColumnFlags_WidthFixed, smallColumn);
        ImGui::TableSetupColumn("Mode", ImGuiTableColumnFlags_PreferSortDescending | ImGuiTableColumnFlags_WidthStretch, smallColumn);
        ImGui::TableSetupScrollFreeze(0, 1); // Make row always visible
        ImGui::TableHeadersRow();

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

            ImGui::PushID(addr);
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            if (module)
                ImGui::Text("%s", module->getName().c_str());
            else
                ImGui::Text("Module: %d", ms.address);
            ImGui::TableNextColumn();
            ImGui::Text("%d", ms.address);
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

void MonitorWindow::drawMenubar()
{
    if (ImGui::BeginMenuBar()) {

        if (ImGui::MenuItem("Clear")) {
            AppManager::getCanBusStream().getModuleStatuses().clear();
            AppManager::getCanBusStream().getNodeGroupStatuses().clear();
        }

        ImGui::EndMenuBar();
    }
}
}