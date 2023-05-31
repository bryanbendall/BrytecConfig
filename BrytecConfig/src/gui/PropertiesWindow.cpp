#include "PropertiesWindow.h"

#include "AppManager.h"
#include "data/InternalPin.h"
#include "data/Module.h"
#include "data/PhysicalPin.h"
#include "data/Pin.h"
#include <IconsFontAwesome5.h>
#include <imgui.h>
#include <memory>
#include <misc/cpp/imgui_stdlib.h>

namespace Brytec {

static ImGuiTableFlags flags = ImGuiTableFlags_Resizable | ImGuiTableFlags_NoBordersInBody;

void PropertiesWindow::drawWindow()
{
    if (!m_opened)
        return;

    ImGui::Begin(ICON_FA_COG " Properties", &m_opened);

    std::shared_ptr<Selectable> selected = AppManager::getSelectedItem().lock();
    if (selected) {

        if (auto module = AppManager::getSelected<Module>())
            drawModuleProps(module);

        if (auto pin = AppManager::getSelected<Pin>()) {
            drawPinProps(pin);

            if (auto nodeGroup = pin->getNodeGroup())
                drawNodeGroupProps(nodeGroup);
        }

        if (auto nodeGroup = AppManager::getSelected<NodeGroup>())
            drawNodeGroupProps(nodeGroup);
    }

    ImGui::End();
}

void PropertiesWindow::drawModuleProps(std::shared_ptr<Module> module)
{

    if (ImGui::CollapsingHeader("Module", ImGuiTreeNodeFlags_DefaultOpen)) {

        if (ImGui::BeginTable("ModuleProps", 2, flags)) {

            ImGui::TableSetupColumn("Module", ImGuiTableColumnFlags_NoHide | ImGuiTableColumnFlags_WidthFixed, 100.0f);

            // Name
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::AlignTextToFramePadding();
            ImGui::TextUnformatted("Name");
            ImGui::TableNextColumn();
            ImGui::SetNextItemWidth(-FLT_MIN);
            ImGui::InputText("###MouduleName", &module->getName(), ImGuiInputTextFlags_AutoSelectAll);

            // Manufacturer Name
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::AlignTextToFramePadding();
            ImGui::TextUnformatted("Manufacturer");
            ImGui::TableNextColumn();
            ImGui::SetNextItemWidth(-FLT_MIN);
            ImGui::TextUnformatted(module->getManufacturerName().c_str());

            // Board Name
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::AlignTextToFramePadding();
            ImGui::TextUnformatted("Board");
            ImGui::TableNextColumn();
            ImGui::SetNextItemWidth(-FLT_MIN);
            ImGui::TextUnformatted(module->getBoardName().c_str());

            // Address
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::AlignTextToFramePadding();
            ImGui::TextUnformatted("Address");
            ImGui::TableNextColumn();
            static bool showButtons = true;
            ImGui::SetNextItemWidth(-FLT_MIN);
            ImGui::InputScalar("###ModuleAddressInput", ImGuiDataType_U8, &module->getAddress(), &showButtons);

            // Enabled
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::AlignTextToFramePadding();
            ImGui::TextUnformatted("Enabled");
            ImGui::TableNextColumn();
            ImGui::SetNextItemWidth(-FLT_MIN);
            ImGui::Checkbox("###MouduleEnabled", &module->getEnabled());

            // Node and NodeGroup Statistics
            int totalNodes = 0;
            int totalNodeGroups = 0;
            int enabledNodeGroups = 0;
            for (auto& pin : module->getPhysicalPins()) {
                if (auto ng = pin->getNodeGroup()) {
                    totalNodes += ng->getNodes().size();
                    totalNodeGroups++;
                    if (ng->getEnabled())
                        enabledNodeGroups++;
                }
            }
            for (auto& pin : module->getInternalPins()) {
                if (auto ng = pin->getNodeGroup()) {
                    totalNodes += ng->getNodes().size();
                    totalNodeGroups++;
                    if (ng->getEnabled())
                        enabledNodeGroups++;
                }
            }

            // Total Nodes
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::AlignTextToFramePadding();
            ImGui::TextUnformatted("Total Nodes");
            ImGui::TableNextColumn();
            ImGui::SetNextItemWidth(-FLT_MIN);
            ImGui::Text("%d", totalNodes);

            // Total NodeGroups
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::AlignTextToFramePadding();
            ImGui::TextUnformatted("Node Groups");
            ImGui::TableNextColumn();
            ImGui::SetNextItemWidth(-FLT_MIN);
            ImGui::Text("%d", totalNodeGroups);

            // Total Enabled NodeGroups
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::AlignTextToFramePadding();
            ImGui::TextUnformatted("Enabled Node Groups");
            ImGui::TableNextColumn();
            ImGui::SetNextItemWidth(-FLT_MIN);
            ImGui::Text("%d", enabledNodeGroups);

            ImGui::EndTable();

            for (auto& canBus : module->getCanBuss()) {
                drawCanBus(canBus);
            }
        }
    }
}

void PropertiesWindow::drawCanBus(CanBus& can)
{
    // Can Bus
    if (ImGui::CollapsingHeader(can.name.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {

        if (ImGui::BeginTable("CanProps", 2, flags)) {

            ImGui::TableSetupColumn("CanProps", ImGuiTableColumnFlags_NoHide | ImGuiTableColumnFlags_WidthFixed, 100.0f);

            // Can Type
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::TextUnformatted("Type");
            ImGui::TableNextColumn();
            ImGui::SetNextItemWidth(-FLT_MIN);
            ImGui::Combo("###CanType", (int*)&can.type, CanTypes::Strings, IM_ARRAYSIZE(CanTypes::Strings));

            if (can.type == CanTypes::Types::Custom) {

                // Can Speed
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::TextUnformatted("Speed");
                ImGui::TableNextColumn();
                ImGui::SetNextItemWidth(-FLT_MIN);
                ImGui::Combo("###CanSpeed", (int*)&can.speed, CanSpeed::Strings, IM_ARRAYSIZE(CanSpeed::Strings));

                // Can Format
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::TextUnformatted("Format");
                ImGui::TableNextColumn();
                ImGui::SetNextItemWidth(-FLT_MIN);
                ImGui::Combo("###CanFormat", (int*)&can.format, CanFormat::Strings, IM_ARRAYSIZE(CanFormat::Strings));
            }

            ImGui::EndTable();
        }
    }
}

void PropertiesWindow::drawPinProps(std::shared_ptr<Pin> pin)
{

    if (ImGui::CollapsingHeader("Pin", ImGuiTreeNodeFlags_DefaultOpen)) {
        if (ImGui::BeginTable("PinProps", 2, flags)) {

            ImGui::TableSetupColumn("Pin", ImGuiTableColumnFlags_NoHide | ImGuiTableColumnFlags_WidthFixed, 100.0f);

            auto physicalPin = std::dynamic_pointer_cast<PhysicalPin>(pin);
            if (physicalPin) {

                // Name
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::AlignTextToFramePadding();
                ImGui::TextUnformatted("Pinout Name");
                ImGui::TableNextColumn();
                ImGui::SetNextItemWidth(-FLT_MIN);
                ImGui::TextUnformatted(physicalPin->getPinoutName().c_str());

                // Current Limit
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::AlignTextToFramePadding();
                ImGui::TextUnformatted("Max Current");
                ImGui::TableNextColumn();
                ImGui::SetNextItemWidth(-FLT_MIN);
                if (physicalPin->getMaxCurrent() > 0)
                    ImGui::Text("%i Amps", physicalPin->getMaxCurrent());
                else
                    ImGui::TextUnformatted("N/A");

                // Pwm
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::AlignTextToFramePadding();
                ImGui::TextUnformatted("Pwm");
                ImGui::TableNextColumn();
                ImGui::SetNextItemWidth(-FLT_MIN);
                ImGui::TextUnformatted(physicalPin->getPwm() ? "Yes" : "No");

            } else {
                // Name
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::AlignTextToFramePadding();
                ImGui::TextUnformatted("Pinout Name");
                ImGui::TableNextColumn();
                ImGui::SetNextItemWidth(-FLT_MIN);
                ImGui::TextUnformatted("Internal");
            }

            // Type select
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            if (ImGui::TreeNodeEx("Types", ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_DefaultOpen)) {

                for (auto& type : pin->getAvailableTypes()) {
                    ImGui::PushID((int)type);
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::TableNextColumn();
                    ImGui::AlignTextToFramePadding();
                    ImGui::Text(IOTypes::getString(type), "");

                    ImGui::PopID();
                }

                ImGui::TreePop();
            }

            // Node Group
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::AlignTextToFramePadding();
            ImGui::TextUnformatted("Node Group");
            ImGui::TableNextColumn();
            ImGui::SetNextItemWidth(-FLT_MIN);
            if (pin->getNodeGroup())
                ImGui::TextUnformatted(pin->getNodeGroup()->getName().c_str());
            else {
                // Show combo if there is none selected
                if (ImGui::BeginCombo("###pinsCombo", "")) {

                    for (auto& nodeGroup : AppManager::getConfig()->getNodeGroups()) {

                        // Don't show if node group type is not compatable
                        if (std::find(pin->getAvailableTypes().begin(), pin->getAvailableTypes().end(), nodeGroup->getType()) == pin->getAvailableTypes().end())
                            continue;

                        ImGui::PushID(nodeGroup.get());
                        bool selected = false;
                        if (!nodeGroup->getAssigned()) {
                            if (ImGui::Selectable(nodeGroup->getName().c_str(), &selected))
                                pin->setNodeGroup(nodeGroup);
                        }
                        ImGui::PopID();
                    }
                    ImGui::EndCombo();
                }
            }

            // Remove button
            if (pin->getNodeGroup()) {
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::TableNextColumn();
                if (ImGui::Button("Remove Node Group"))
                    pin->setNodeGroup(nullptr);
            }

            ImGui::EndTable();
        }
    }
}

void PropertiesWindow::drawNodeGroupProps(std::shared_ptr<NodeGroup> nodeGroup)
{
    if (ImGui::CollapsingHeader("Node Group", ImGuiTreeNodeFlags_DefaultOpen)) {
        if (ImGui::BeginTable("NodeGroupProps", 2, flags)) {

            ImGui::TableSetupColumn("Node Group", ImGuiTableColumnFlags_NoHide | ImGuiTableColumnFlags_WidthFixed, 100.0f);

            // Name
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::AlignTextToFramePadding();
            ImGui::TextUnformatted("Name");
            ImGui::TableNextColumn();
            ImGui::SetNextItemWidth(-FLT_MIN);
            ImGui::InputText("###pinName", &nodeGroup->getName(), ImGuiInputTextFlags_AutoSelectAll);

            // Id
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::AlignTextToFramePadding();
            ImGui::TextUnformatted("Id");
            ImGui::TableNextColumn();
            ImGui::SetNextItemWidth(-FLT_MIN);
            ImGui::Text("%llu", (unsigned long long)nodeGroup->getId());

            // Enabled
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::AlignTextToFramePadding();
            ImGui::TextUnformatted("Enabled");
            ImGui::TableNextColumn();
            ImGui::SetNextItemWidth(-FLT_MIN);
            ImGui::Checkbox("###MouduleEnabled", &nodeGroup->getEnabled());

            // Used on bus indicator
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::AlignTextToFramePadding();
            ImGui::TextUnformatted("Used On Bus");
            ImGui::TableNextColumn();
            ImGui::SetNextItemWidth(-FLT_MIN);
            if (AppManager::getConfig()->getUsedOnBus(nodeGroup))
                ImGui::TextUnformatted(ICON_FA_CHECK);

            // Type
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::AlignTextToFramePadding();
            ImGui::TextUnformatted("Type");
            ImGui::TableNextColumn();
            ImGui::SetNextItemWidth(-FLT_MIN);
            if (nodeGroup->getAssigned())
                ImGui::Text("%s %s", ICON_FA_LOCK, IOTypes::getString(nodeGroup->getType()));
            else
                ImGui::Combo("###Node Group Type", (int*)&nodeGroup->getType(), IOTypes::Strings, IM_ARRAYSIZE(IOTypes::Strings), 10);

            // Current limit
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::AlignTextToFramePadding();
            ImGui::TextUnformatted("Current Limit");
            ImGui::TableNextColumn();
            ImGui::SetNextItemWidth(-FLT_MIN);
            int current = nodeGroup->getCurrentLimit();
            if (ImGui::DragInt("###current", &current, 1.0f, 0, 255, "%d Amps"))
                nodeGroup->setCurrentLimit(std::clamp(current, 0, 255));

            // Always Retry
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::AlignTextToFramePadding();
            ImGui::TextUnformatted("Always Retry");
            ImGui::TableNextColumn();
            ImGui::SetNextItemWidth(-FLT_MIN);
            ImGui::Checkbox("###AlwayRetry", &nodeGroup->getAlwaysRetry());

            // Maximum Retries
            if (!nodeGroup->getAlwaysRetry()) {
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::AlignTextToFramePadding();
                ImGui::TextUnformatted("Retries");
                ImGui::TableNextColumn();
                ImGui::SetNextItemWidth(-FLT_MIN);
                int retries = nodeGroup->getMaxRetries();
                if (ImGui::DragInt("###MaximumRetries", &retries, 1.0f, 0, 255))
                    nodeGroup->setMaxRetries(std::clamp(retries, 0, 255));
            }

            // Retry delay
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::AlignTextToFramePadding();
            ImGui::TextUnformatted("Retry Delay");
            ImGui::TableNextColumn();
            ImGui::SetNextItemWidth(-FLT_MIN);
            ImGui::DragFloat("##RetryDelay", &nodeGroup->getRetryDelay(), 0.1f, 0.1f, 10.0f, "%.1f Sec");

            // Total Nodes
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::AlignTextToFramePadding();
            ImGui::TextUnformatted("Total Nodes");
            ImGui::TableNextColumn();
            ImGui::SetNextItemWidth(-FLT_MIN);
            ImGui::Text("%d", (int)nodeGroup->getNodes().size());

            ImGui::EndTable();
        }
    }
}

}