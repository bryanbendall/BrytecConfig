#include "PropertiesWindow.h"

#include "AppManager.h"
#include <IconsFontAwesome5.h>
#include <imgui.h>
#include <memory>
#include <misc/cpp/imgui_stdlib.h>

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

            // Total Nodes
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::AlignTextToFramePadding();
            ImGui::TextUnformatted("Total Nodes");
            ImGui::TableNextColumn();
            ImGui::SetNextItemWidth(-FLT_MIN);
            int totalNodes = 0;
            for (auto& pin : module->getPhysicalPins()) {
                if (auto ng = pin->getNodeGroup())
                    totalNodes += ng->getNodes().size();
            }
            for (auto& pin : module->getInternalPins()) {
                if (auto ng = pin->getNodeGroup())
                    totalNodes += ng->getNodes().size();
            }
            ImGui::Text("%d", totalNodes);

            // ImGui::TableNextRow();
            // ImGui::TableNextColumn();
            // ImGui::AlignTextToFramePadding();
            // ImGui::Text("Can 0");

            // ImGui::TableNextColumn();

            // ImGui::SetNextItemWidth(-FLT_MIN);
            // const char* items[] = { "Disabled", "Brytec", "Custom" };
            // static int item_current_idx = 1; // Here we store our selection data as an index.
            // const char* combo_preview_value = items[item_current_idx]; // Pass in the preview value visible before opening the combo (it could be anything)
            // if (ImGui::BeginCombo("###combo 1", combo_preview_value)) {
            //     for (int n = 0; n < IM_ARRAYSIZE(items); n++) {
            //         const bool is_selected = (item_current_idx == n);
            //         if (ImGui::Selectable(items[n], is_selected))
            //             item_current_idx = n;

            //         // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
            //         if (is_selected)
            //             ImGui::SetItemDefaultFocus();
            //     }
            //     ImGui::EndCombo();
            // }

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

            if (pin->getNodeGroup()) {
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::TableNextColumn();
                if (ImGui::Button("Remove Node Group"))
                    pin->setNodeGroup(nullptr);
            }

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

            // Type
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::AlignTextToFramePadding();
            ImGui::TextUnformatted("Type");
            ImGui::TableNextColumn();
            ImGui::SetNextItemWidth(-FLT_MIN);
            if (nodeGroup->getAssigned()) {
                ImGui::Text("%s %s", ICON_FA_LOCK, IOTypes::getString(nodeGroup->getType()));
            } else {
                ImGui::Combo("###Node Group Type", (int*)&nodeGroup->getType(), IOTypes::Strings, IM_ARRAYSIZE(IOTypes::Strings), 10);
            }

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
