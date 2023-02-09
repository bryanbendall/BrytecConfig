#include "NodeGroupWindow.h"

#include "AppManager.h"
#include "utils/Colors.h"
#include "utils/DefaultPaths.h"
#include "utils/FileDialogs.h"
#include "utils/NodeGroupSerializer.h"
#include <IconsFontAwesome5.h>
#include <imgui.h>
#include <iostream>
#include <misc/cpp/imgui_stdlib.h>

NodeGroupWindow::NodeGroupWindow()
{
}

void NodeGroupWindow::drawWindow()
{
    if (!m_opened)
        return;
    ImGui::Begin(ICON_FA_DICE_D6 " Node Group", &m_opened, ImGuiWindowFlags_MenuBar);

    if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(0) && !ImGui::IsAnyItemHovered())
        AppManager::setSelected(std::weak_ptr<Selectable>());

    drawMenubar();

    drawNodeGroups();

    handleDragDrop();

    ImGui::End();
}

void NodeGroupWindow::drawMenubar()
{
    if (ImGui::BeginMenuBar()) {

        std::shared_ptr<NodeGroup> nodeGroup = AppManager::getSelected<NodeGroup>();
        bool nodeGroupSelected = nodeGroup != nullptr;

        // Add blank
        if (ImGui::MenuItem(ICON_FA_PLUS_CIRCLE))
            AppManager::setSelected(AppManager::getConfig()->addNodeGroup());

        // Open
        if (ImGui::MenuItem(ICON_FA_FOLDER_OPEN)) {
            auto path = FileDialogs::OpenFile("btnodes", NODE_GROUPS_PATH);
            if (!path.empty()) {
                std::shared_ptr<NodeGroup> nodeGroup = AppManager::getConfig()->addEmptyNodeGroup(UUID());
                NodeGroupSerializer serializer(nodeGroup);
                BinaryDeserializer des;
                des.setDataFromPath(path);
                if (serializer.deserializeBinary(des)) {
                    AppManager::setSelected(nodeGroup);
                } else {
                    std::cout << "Could not deserialize node group file" << std::endl;
                    AppManager::getConfig()->removeNodeGroup(nodeGroup);
                }
            }
        }

        // Save
        if (ImGui::MenuItem(ICON_FA_SAVE, NULL, false, nodeGroupSelected)) {
            if (nodeGroup) {
                auto path = FileDialogs::SaveFile("btnodes", NODE_GROUPS_PATH);

                if (path.extension().empty())
                    path.replace_extension("btnodes");

                if (!path.empty()) {
                    NodeGroupSerializer serializer(nodeGroup);
                    BinarySerializer binary = serializer.serializeBinary();
                    binary.writeToFile(path);
                }
            }
        }

        // Copy Node Group
        if (ImGui::MenuItem(ICON_FA_COPY, NULL, false, nodeGroupSelected)) {
            // Copy Constructor
            auto newNodeGroup = std::make_shared<NodeGroup>(*nodeGroup.get());
            AppManager::getConfig()->addNodeGroup(newNodeGroup);
            AppManager::setSelected(newNodeGroup);
        }

        ImGui::TextDisabled("|");

        // Filter
        ImGui::Text(ICON_FA_FILTER " Filter");
        ImGui::SetNextItemWidth(110);
        static const char* items[] = { "All", "Assigned", "Unassigned" };
        ImGui::Combo("##Filter", (int*)&m_filter, items, 3);

        ImGui::TextDisabled("|");

        // Search
        ImGui::Text("Search");
        ImGui::SetNextItemWidth(110);
        ImGui::InputText("###Search", &m_search, ImGuiInputTextFlags_AutoSelectAll);

        int hidden = AppManager::getConfig()->getNodeGroups().size() - m_shownNodeGroups;
        if (hidden) {
            ImGui::TextDisabled("|");
            ImGui::Text(ICON_FA_EXCLAMATION_TRIANGLE " %d Hidden", hidden);
        }

        ImGui::EndMenuBar();
    }
}

void NodeGroupWindow::drawNodeGroups()
{
    auto& nodeGroups = AppManager::getConfig()->getNodeGroups();
    int nodeGroupCount = nodeGroups.size();
    float window_visible_x2 = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;
    float buttonWidth = 150;

    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.0f);

    m_shownNodeGroups = 0;

    for (int i = 0; i < nodeGroupCount; i++) {
        std::shared_ptr<NodeGroup> nodeGroup = nodeGroups[i];

        switch (m_filter) {
        case FilterType::All:
            break;
        case FilterType::Assigned:
            if (!nodeGroup->getAssigned())
                continue;
            break;
        case FilterType::Unassigned:
            if (nodeGroup->getAssigned())
                continue;
            break;
        }

        if (!m_search.empty()) {
            // Filter - not case sensitive
            auto it = std::search(
                nodeGroup->getName().begin(), nodeGroup->getName().end(),
                m_search.begin(), m_search.end(),
                [](unsigned char ch1, unsigned char ch2) { return std::toupper(ch1) == std::toupper(ch2); });
            if (it == nodeGroup->getName().end())
                continue;
        }

        m_shownNodeGroups++;

        ImGui::PushID(i);

        bool selected = AppManager::isSelected(nodeGroup);

        if (selected) {
            ImGui::PushStyleColor(ImGuiCol_Border, Colors::PrimarySelection);
            ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
        }

        bool pinSelected = false;
        auto pin = AppManager::getSelected<Pin>();
        if (pin && pin->getNodeGroup() == nodeGroup) {
            pinSelected = true;
            ImGui::PushStyleColor(ImGuiCol_Border, Colors::SecondarySelection);
            ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
        }

        std::string displayName = nodeGroup->getName();
        if (nodeGroup->getAssigned()) {
            displayName += " ";
            displayName += ICON_FA_LOCK;
        }
        if (ImGui::Button(displayName.c_str(), { buttonWidth, 0 })) {
            AppManager::setSelected(nodeGroup);
        }

        if (!nodeGroup->getAssigned()) {
            if (ImGui::BeginDragDropSource()) {
                uint64_t id = nodeGroup->getId();
                ImGui::SetDragDropPayload("NodeGroup", &id, sizeof(uint64_t));

                ImGui::Text(nodeGroup->getName().c_str(), "");
                ImGui::TextDisabled("%s", IOTypes::getString(nodeGroup->getType()));

                AppManager::setDragType(nodeGroup->getType());

                ImGui::EndDragDropSource();
            }
        }

        if (selected || pinSelected) {
            ImGui::PopStyleVar();
            ImGui::PopStyleColor();
        }

        // Flow layout
        ImGuiStyle& style = ImGui::GetStyle();
        float last_button_x2 = ImGui::GetItemRectMax().x;
        float next_button_x2 = last_button_x2 + style.ItemSpacing.x + buttonWidth; // Expected position if next button was on same line
        if (i + 1 < nodeGroupCount && next_button_x2 < window_visible_x2)
            ImGui::SameLine();

        ImGui::PopID();
    }

    ImGui::PopStyleVar();
}

void NodeGroupWindow::handleDragDrop()
{
    ImGui::SetCursorPos(ImGui::GetCursorStartPos());
    ImGui::Dummy(ImGui::GetContentRegionAvail());

    // Drop Node Group
    if (ImGui::BeginDragDropTarget()) {

        // Check type to do the outline for drop
        bool accepted = false;
        if (const ImGuiPayload* tempPayload = ImGui::AcceptDragDropPayload("NodeGroup", ImGuiDragDropFlags_AcceptPeekOnly)) {
            uint64_t uuid = *(uint64_t*)tempPayload->Data;
            std::shared_ptr<NodeGroup> nodeGroup = AppManager::getConfig()->findNodeGroup(uuid);
            if (nodeGroup) {
                accepted = true;
            }
        }

        // Recieve the data if it is a compatible type
        if (accepted) {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("NodeGroup")) {

                uint64_t uuid = *(uint64_t*)payload->Data;
                std::shared_ptr<NodeGroup> nodeGroup = AppManager::getConfig()->findNodeGroup(uuid);

                std::shared_ptr<Pin> oldPin = AppManager::getConfig()->getAssignedPin(nodeGroup);
                if (oldPin)
                    oldPin->setNodeGroup(nullptr);
            }
        }
        ImGui::EndDragDropTarget();
    }
}