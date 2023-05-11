#include "SettingsWindow.h"

#define IMGUI_DEFINE_MATH_OPERATORS

#include "AppManager.h"
#include "imgui_internal.h"
#include "utils/FileDialogs.h"
#include <IconsFontAwesome5.h>
#include <filesystem>
#include <fstream>
#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

namespace Brytec {

SettingsWindow::SettingsWindow()
{
}

void SettingsWindow::drawWindow()
{
    if (!m_opened)
        return;

    ImGui::SetNextWindowSize(ImVec2(500.0f, 500.0f), ImGuiCond_Appearing);
    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->Size * 0.5f, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    ImGui::Begin(ICON_FA_COG " Settings", &m_opened, ImGuiWindowFlags_NoDocking);

    if (ImGui::BeginTable("Settings", 2, ImGuiTableFlags_Resizable | ImGuiTableFlags_NoBordersInBody)) {

        ImGui::TableSetupColumn("SettingsCol", ImGuiTableColumnFlags_NoHide | ImGuiTableColumnFlags_WidthFixed, 200.0f);

        // Zoom
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted("Zoom");
        ImGui::TableNextColumn();
        ImGui::SetNextItemWidth(-FLT_MIN);
        int zoom = AppManager::getZoom();
        if (ImGui::InputInt("##input", &zoom, 2))
            AppManager::setZoom(zoom);

        // Configs Path
        ImGui::PushID("Config");
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted("Config Path");
        ImGui::TableNextColumn();
        ImGui::SetNextItemWidth(-FLT_MIN);
        if (ImGui::Button(AppManager::getConfigsPath().string().c_str())) {
            std::filesystem::path path = FileDialogs::PickFolder(AppManager::getConfigsPath());
            if (!path.empty())
                AppManager::setConfigsPath(path);
        }
        ImGui::PopID();

        // Modules Path
        ImGui::PushID("Module");
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted("Module Path");
        ImGui::TableNextColumn();
        ImGui::SetNextItemWidth(-FLT_MIN);
        if (ImGui::Button(AppManager::getModulesPath().string().c_str())) {
            std::filesystem::path path = FileDialogs::PickFolder(AppManager::getModulesPath());
            if (!path.empty())
                AppManager::setModulesPath(path);
        }
        ImGui::PopID();

        // Node Groups Path
        ImGui::PushID("Node Groups");
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted("Node Groups Path");
        ImGui::TableNextColumn();
        ImGui::SetNextItemWidth(-FLT_MIN);
        if (ImGui::Button(AppManager::getNodeGroupsPath().string().c_str())) {
            std::filesystem::path path = FileDialogs::PickFolder(AppManager::getNodeGroupsPath());
            if (!path.empty())
                AppManager::setNodeGroupsPath(path);
        }
        ImGui::PopID();

        // Open Last File
        ImGui::PushID("Open Last File");
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted("Open Last File");
        ImGui::TableNextColumn();
        ImGui::SetNextItemWidth(-FLT_MIN);
        bool value = AppManager::getOpenLastFile();
        if (ImGui::Checkbox("##Open Last File", &value))
            AppManager::setOpenLastFile(value);
        ImGui::PopID();

        ImGui::EndTable();
    }

    ImGui::End();
}

}