#include "MainWindow.h"

#include <imgui.h>
//#include <misc/freetype/imgui_freetype.h>
#include <backends/imgui_impl_opengl3.h>
#include <IconsFontAwesome5.h>

MainWindow::MainWindow(GLFWwindow* window) 
    : m_window(window)
{

}

void MainWindow::setupFonts() 
{
    ImGui::GetStyle();
    // Load Fonts
    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->AddFontFromFileTTF("vendor\\imgui\\misc\\fonts\\DroidSans.ttf", 16.0f);

    static const ImWchar icons_ranges[] = {ICON_MIN_FA, ICON_MAX_FA, 0};
    ImFontConfig icons_config;
    icons_config.MergeMode = true;
    icons_config.PixelSnapH = true;
    io.Fonts->AddFontFromFileTTF("vendor\\fontawesome\\fa-solid-900.ttf", 14.0f, &icons_config, icons_ranges);
    //ImGuiFreeType::BuildFontAtlas(io.Fonts, ImGuiFreeType::ForceAutoHint | ImGuiFreeType::MonoHinting);
    ImGui_ImplOpenGL3_DestroyDeviceObjects();
    ImGui_ImplOpenGL3_CreateDeviceObjects();
}

void MainWindow::setupStyle() 
{
    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    ImGuiStyle* style = &ImGui::GetStyle();
    style->WindowMenuButtonPosition = ImGuiDir_None;
    style->Colors[ImGuiCol_TitleBgActive] = style->Colors[ImGuiCol_TitleBg];
}

void MainWindow::loadLayout() 
{
    if(m_ini_to_load) {
        ImGui::LoadIniSettingsFromDisk(m_ini_to_load);
        m_ini_to_load = NULL;
    }
}

void MainWindow::drawWindow()
{
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->GetWorkPos());
    ImGui::SetNextWindowSize(viewport->GetWorkSize());
    ImGui::SetNextWindowViewport(viewport->ID);

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize
        | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus
        | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    static bool p_open = true;
    ImGui::Begin("DockSpace", &p_open, window_flags);
    ImGui::PopStyleVar();
    ImGui::PopStyleVar();

    drawMenuBar();

    // DockSpace
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
    ImGui::DockSpace(ImGui::GetID("MyDockSpace"), ImVec2(0.0f, 0.0f), dockspace_flags);

    drawMenu();

    m_moduleWindow.drawWindow();
    m_propertiesWindow.drawWindow();
    m_nodeWindow.drawWindow();

    ImGui::End();
}

void MainWindow::drawMenu() 
{
    if(ImGui::BeginMenuBar()) {
        if(ImGui::BeginMenu("File")) {
            if(ImGui::MenuItem("Exit"))                     glfwSetWindowShouldClose(m_window, 1);
            ImGui::EndMenu();
        }

        if(ImGui::BeginMenu("View")) {
            if(ImGui::MenuItem("Open Module Window"))       m_moduleWindow.setOpenedState(true);
            //if(ImGui::MenuItem("Open Node Window"))         m_nodeWindow.setOpenedState(true);
            if(ImGui::MenuItem("Open Properties Window"))   m_propertiesWindow.setOpenedState(true);
            ImGui::EndMenu();
        }

        if(ImGui::BeginMenu("Layout")) {
            if(ImGui::MenuItem("Pinout")) m_ini_to_load = "imguiPinLayout.ini";
            if(ImGui::MenuItem("Node"))   m_ini_to_load = "imguiNodeLayout.ini";
            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }
}

void MainWindow::drawMenuBar() 
{
    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::SameLine();
    ImGui::Button(ICON_FA_DICE_D6);
    ImGui::SameLine();
    bool sel = false;
    ImGui::Selectable(ICON_FA_DICE_D6, &sel, 0, ImVec2(ImGui::CalcTextSize(ICON_FA_DICE_D6).x, 0));
    ImGui::SameLine();

    if(ImGui::Button("popup")) {
        ImGui::OpenPopup("popup!");
    }

    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(300.0f, 400.0f));
    if(ImGui::BeginPopupModal("popup!", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        if(ImGui::Button("Close")) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}
