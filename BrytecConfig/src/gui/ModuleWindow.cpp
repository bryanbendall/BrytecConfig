#include "ModuleWindow.h"
#include "../AppManager.h"
#include <iostream>
#include <IconsFontAwesome5.h>
#include "../utils/ModuleBuilder.h"
#include <filesystem>

ModuleWindow::ModuleWindow() {

}

void ModuleWindow::drawWindow() {
    if (!m_opened)
        return;
	ImGui::Begin(ICON_FA_DICE_D6" Modules", &m_opened, ImGuiWindowFlags_MenuBar);
    drawMenubar();
    if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(0) && !ImGui::IsAnyItemHovered())
        AppManager::setSelected(std::weak_ptr<Selectable>());
    drawModules();
	ImGui::End();
}

void ModuleWindow::drawMenubar() 
{
    if(ImGui::BeginMenuBar()) {

        if(ImGui::BeginMenu("Add")) {
            auto moduleList = ModuleBuilder::readModulesFromDisk();
            for(auto& modulePath : moduleList) {
                if(ImGui::MenuItem(modulePath.stem().string().c_str()))
                    AppManager::getConfig().addModule(modulePath);
            }
            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }

}

void ModuleWindow::drawModules()
{
    unsigned int moduleCount = AppManager::getConfig().getModules().size();
    float nextSizeX = 150;
    ImGuiStyle& style = ImGui::GetStyle();
    float window_visible_x2 = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;

    //ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.0f);
    for (unsigned int n = 0; n < moduleCount; n++)
    {
        ImGui::PushID(n);
        std::shared_ptr<Module> m = AppManager::getConfig().getModules()[n];
        drawModule(m);
        //ImGui::Button("i/o", ImVec2(150, 25));
        float last_button_x2 = ImGui::GetItemRectMax().x;
        float next_button_x2 = last_button_x2 + style.ItemSpacing.x + nextSizeX; // Expected position if next button was on same line
        if (n + 1 < moduleCount && next_button_x2 < window_visible_x2)
            ImGui::SameLine();
        ImGui::PopID();
    }
    //ImGui::PopStyleVar();

}

void ModuleWindow::drawModule(std::shared_ptr<Module>& m)
{
    unsigned int numPins = m->getPins().size();

    bool selected = std::dynamic_pointer_cast<Module>(AppManager::getSelectedItem().lock()) == m;

    // Split output to draw background after
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    drawList->ChannelsSplit(2);
    drawList->ChannelsSetCurrent(1);
    
    // Module style setup
    bool titleHovered = false;
    ImVec2 label_size = ImGui::CalcTextSize("Module");
    float titleBarHeight = label_size.y + ImGui::GetStyle().FramePadding.y * 2.0f;

    ImGui::BeginGroup();
    if(!m->getEnabled()) 
        ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyle().Colors[ImGuiCol_TextDisabled]);
    ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(0, 0, 0, 0));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(0, 0, 0, 0));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, IM_COL32(0, 0, 0, 0));
    
    // Draw Module button
    if (ImGui::Button(m->getName().c_str(), { 150, titleBarHeight })) 
        AppManager::setSelected(m);
    titleHovered = ImGui::IsItemHovered();

    // Module style end
    ImGui::PopStyleColor();
    ImGui::PopStyleColor();
    ImGui::PopStyleColor();
    
    // Pin button style setup
    if (!m->getEnabled()) 
        ImGui::PopStyleColor();
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.0f);
    ImGui::Indent(5.0f);
    ImGui::SetNextItemWidth(130.0f);

    // Draw Pin buttons
    for (unsigned int i = 0; i < numPins; i++) {
        ImGui::PushID(i);
        auto pin = m->getPins()[i];
        bool enabled = pin->getNodeGroup() ? pin->getNodeGroup()->getEnabled() : false;
        std::string buttonText = pin->getNodeGroup() ? pin->getNodeGroup()->getName() : pin->getPinoutName();
        if (!enabled)
            ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyle().Colors[ImGuiCol_TextDisabled]);
        if(ImGui::Button(buttonText.c_str(), { 140, 0 }))
            AppManager::setSelected(pin);
        if (!enabled)
            ImGui::PopStyleColor();

        // Selection boarder
        if (pin == std::dynamic_pointer_cast<Pin>(AppManager::getSelectedItem().lock())) {
            ImVec2 rectMin = ImGui::GetItemRectMin();
            ImVec2 rectMax = ImGui::GetItemRectMax();
            drawList->AddRect(rectMin, rectMax, IM_COL32_WHITE, 4.0f);
        }

        // Drop Node Group
        if(ImGui::BeginDragDropTarget()) {

            // Check type to do the outline for drop
            bool accepted = false;
            if(const ImGuiPayload * tempPayload = ImGui::AcceptDragDropPayload("NodeGroup", ImGuiDragDropFlags_AcceptPeekOnly)) {
                int nodeGroupIndex = *(int*) tempPayload->Data;
                auto& nodeGroup = AppManager::getConfig().getNodeGroups()[nodeGroupIndex];
                if(std::find(pin->getAvailableTypes().begin(), pin->getAvailableTypes().end(), nodeGroup->getType()) != pin->getAvailableTypes().end()) {
                    accepted = true;
                }
            }

            // Recieve the data if it is a compatible type
            if(accepted) {
                if(const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("NodeGroup")) {

                    // TODO: check if it is right type?
                    int nodeGroupIndex = *(int*) payload->Data;
                    auto& nodeGroup = AppManager::getConfig().getNodeGroups()[nodeGroupIndex];
                    pin->setNodeGroup(nodeGroup);
                }
            }
            ImGui::EndDragDropTarget();
        }

        ImGui::PopID();
    }

    // Pin style end
    ImGui::Spacing();
    ImGui::PopStyleVar();
    ImGui::EndGroup();

    // Draw background
    ImVec2 rectMin = ImGui::GetItemRectMin();
    ImVec2 rectMax = ImGui::GetItemRectMax();
    
    drawList->ChannelsSetCurrent(0);
    // node base
    drawList->AddRectFilled(
        rectMin,
        rectMax,
        ColorStyle_Background,
        4);
    // title bar:
    drawList->AddRectFilled(
        rectMin,
        { rectMax.x, rectMin.y + titleBarHeight },
        titleHovered ? ColorStyle_TitleBarHovered : ColorStyle_TitleBar,
        4,
        ImDrawCornerFlags_Top);
    // outline
    drawList->AddRect(
        rectMin,
        rectMax,
        selected ? IM_COL32_WHITE : ColorStyle_Outline,
        4);

    drawList->ChannelsMerge();
}
