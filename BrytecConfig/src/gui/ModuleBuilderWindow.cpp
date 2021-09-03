#include "ModuleBuilderWindow.h"

#include <imgui.h>

ModuleBuilderWindow::ModuleBuilderWindow()
{
}

void ModuleBuilderWindow::drawWindow()
{
    if(m_showWindow) {
        ImGui::OpenPopup("Module Builder");
        m_showWindow = false;
    }

    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(300.0f, 200.0f));
    if(ImGui::BeginPopupModal("Module Builder", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        if(ImGui::Button("Close")) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}
