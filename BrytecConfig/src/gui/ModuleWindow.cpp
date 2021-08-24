#include "ModuleWindow.h"
#include "../AppManager.h"
#include <iostream>
#include <IconsFontAwesome5.h>

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

            for(int i = 0; i < (size_t)ModuleTypes::Count; i++) {// auto moduleTypeName : Module::typeNames) {
                if(ImGui::MenuItem(Module::typeNames[i].c_str())) {
                    AppManager::getConfig().addModule((ModuleTypes)i);
                }
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
    for (unsigned int n = 0; n < moduleCount; n++)
    {
        ImGui::PushID(n);
        std::shared_ptr<Module> m = AppManager::getConfig().getModules()[n];
        drawModule(m);
        float last_button_x2 = ImGui::GetItemRectMax().x;
        float next_button_x2 = last_button_x2 + style.ItemSpacing.x + nextSizeX; // Expected position if next button was on same line
        if (n + 1 < moduleCount && next_button_x2 < window_visible_x2)
            ImGui::SameLine();
        ImGui::PopID();
    }
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
    
    // Drag Module button
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
        if (!m->getPins()[i]->getEnabled()) 
            ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyle().Colors[ImGuiCol_TextDisabled]);
        if(ImGui::Button(m->getPins()[i].get()->getName().c_str(), { 140, 0 })) 
            AppManager::setSelected(m->getPins()[i]);        
        if (!m->getPins()[i]->getEnabled()) 
            ImGui::PopStyleColor();

        // Selection boarder
        if (m->getPins()[i] == std::dynamic_pointer_cast<Pin>(AppManager::getSelectedItem().lock())) {
            ImVec2 rectMin = ImGui::GetItemRectMin();
            ImVec2 rectMax = ImGui::GetItemRectMax();
            drawList->AddRect(rectMin, rectMax, IM_COL32_WHITE, 4.0f);
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
