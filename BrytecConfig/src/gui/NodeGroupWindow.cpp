#include "NodeGroupWindow.h"

#include "AppManager.h"
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

    ImGui::End();
}

void NodeGroupWindow::drawMenubar()
{
    if (ImGui::BeginMenuBar()) {

        std::shared_ptr<NodeGroup> nodeGroup = std::dynamic_pointer_cast<NodeGroup>(AppManager::getSelectedItem().lock());
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

                // TODO
                // if (!serializer.deserializeTemplateText(path))
                //     std::cout << "Could not deserialize node group file" << std::endl;
                // else
                //     AppManager::setSelected(nodeGroup);
            }
        }

        // Save
        if (ImGui::MenuItem(ICON_FA_SAVE, NULL, false, nodeGroupSelected)) {
            if (nodeGroup) {
                auto path = FileDialogs::SaveFile("btnodes", NODE_GROUPS_PATH);

                if (path.extension().empty())
                    path.replace_extension("btnodes");

                if (!path.empty()) {
                    // TODO
                    // NodeGroupSerializer serializer(nodeGroup);
                    // serializer.serializeTemplateText(path);
                }
            }
        }

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
        auto& nodeGroup = nodeGroups[i];

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
            // To Lower Case
            std::string name = nodeGroup->getName();
            std::transform(name.begin(), name.end(), name.begin(),
                [](unsigned char c) { return std::tolower(c); });

            // To Lower Case
            std::string search = m_search;
            std::transform(search.begin(), search.end(), search.begin(),
                [](unsigned char c) { return std::tolower(c); });

            if (name.find(search) > name.length()) {
                continue;
            }
        }

        m_shownNodeGroups++;

        ImGui::PushID(i);

        bool selected = std::dynamic_pointer_cast<NodeGroup>(AppManager::getSelectedItem().lock()) == nodeGroup;

        if (selected) {
            ImGui::PushStyleColor(ImGuiCol_Border, IM_COL32(255, 255, 255, 255));
            ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
        }

        if (ImGui::Button(nodeGroup->getName().c_str(), { buttonWidth, 0 })) {
            AppManager::setSelected(nodeGroup);
        }

        if (!nodeGroup->getAssigned()) {
            if (ImGui::BeginDragDropSource()) {
                ImGui::SetDragDropPayload("NodeGroup", &i, sizeof(int));

                ImGui::Text(nodeGroup->getName().c_str(), "");
                ImGui::TextDisabled("%s", IOTypes::getString(nodeGroup->getType()));

                ImGui::EndDragDropSource();
            }
        }

        if (selected) {
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
