#include "NodeGroupWindow.h"

#include <imgui.h>
#include <IconsFontAwesome5.h>
#include "../AppManager.h"
#include <iostream>

NodeGroupWindow::NodeGroupWindow() 
{

}

void NodeGroupWindow::drawWindow()
{
    if(!m_opened)
        return;
    ImGui::Begin(ICON_FA_DICE_D6" Node Group", &m_opened, ImGuiWindowFlags_MenuBar);

    if(ImGui::IsWindowHovered() && ImGui::IsMouseClicked(0) && !ImGui::IsAnyItemHovered())
        AppManager::get()->setSelected(std::weak_ptr<Selectable>());

    drawMenubar();

    drawNodeGroups();

    ImGui::End();
}

void NodeGroupWindow::drawMenubar()
{
    if(ImGui::BeginMenuBar()) {

        if(ImGui::MenuItem(ICON_FA_PLUS_CIRCLE))
            AppManager::get()->getConfig()->addNodeGroup();

        ImGui::EndMenuBar();
    }
}

void NodeGroupWindow::drawNodeGroups()
{
    auto& nodeGroups = AppManager::get()->getConfig()->getNodeGroups();
    int nodeGroupCount = nodeGroups.size();
    float window_visible_x2 = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;
    float buttonWidth = 150;

    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.0f);

    for(int i = 0; i < nodeGroupCount; i++) {
        ImGui::PushID(i);
        auto& nodeGroup = nodeGroups[i];

        bool selected = std::dynamic_pointer_cast<NodeGroup>(AppManager::get()->getSelectedItem().lock()) == nodeGroup;
        
        if(selected) {
            ImGui::PushStyleColor(ImGuiCol_Border, IM_COL32(255, 255, 255, 255));
            ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
        }


        if(ImGui::Button(nodeGroup->getName().c_str(), {buttonWidth, 0})) {
            AppManager::get()->setSelected(nodeGroup);
        }

        if(!nodeGroup->getAssigned()) {
            if(ImGui::BeginDragDropSource()) {
                ImGui::SetDragDropPayload("NodeGroup", &i, sizeof(int));

                ImGui::Text(nodeGroup->getName().c_str());
                ImGui::TextDisabled("%s", IOTypes::Strings[(int) nodeGroup->getType()]);

                ImGui::EndDragDropSource();
            }
        }

        if(selected) {
            ImGui::PopStyleVar();
            ImGui::PopStyleColor();
        }

        // Flow layout
        ImGuiStyle& style = ImGui::GetStyle();
        float last_button_x2 = ImGui::GetItemRectMax().x;
        float next_button_x2 = last_button_x2 + style.ItemSpacing.x + buttonWidth; // Expected position if next button was on same line
        if(i + 1 < nodeGroupCount && next_button_x2 < window_visible_x2)
            ImGui::SameLine();

        ImGui::PopID();
    }

    ImGui::PopStyleVar();
}
