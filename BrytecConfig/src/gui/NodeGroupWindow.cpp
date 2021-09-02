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
    ImGui::Begin(ICON_FA_DICE_D6" I/O", &m_opened);

    if(ImGui::IsWindowHovered() && ImGui::IsMouseClicked(0) && !ImGui::IsAnyItemHovered())
        AppManager::setSelected(std::weak_ptr<Selectable>());

    drawNodeGroups();

    ImGui::End();
}

void NodeGroupWindow::drawNodeGroups()
{
    int nodeGroupCount = AppManager::getConfig().getNodeGroups().size();
    float window_visible_x2 = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;
    float buttonWidth = 150;

    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.0f);

    for(int i = 0; i < nodeGroupCount; i++) {
        ImGui::PushID(i);
        auto& nodeGroup = AppManager::getConfig().getNodeGroups()[i];

        bool selected = std::dynamic_pointer_cast<NodeGroup>(AppManager::getSelectedItem().lock()) == nodeGroup;
        
        if(selected) {
            ImGui::PushStyleColor(ImGuiCol_Border, IM_COL32(255, 255, 255, 255));
            ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
        }
        
        if(ImGui::Button(nodeGroup->getName().c_str(), {buttonWidth, 25})) {
            AppManager::setSelected(nodeGroup);
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