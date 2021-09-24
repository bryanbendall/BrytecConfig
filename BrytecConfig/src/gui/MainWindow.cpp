#include "MainWindow.h"

#include <imgui.h>
//#include <misc/freetype/imgui_freetype.h>
#include <backends/imgui_impl_opengl3.h>
#include <IconsFontAwesome5.h>

MainWindow::MainWindow() 
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
    style->Colors[ImGuiCol_TitleBgActive] = style->Colors[ImGuiCol_TitleBg];

    //setDarkThemeColors();
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
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize
        | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus
        | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    static bool p_open = true;
    ImGui::Begin("DockSpace", &p_open, window_flags);
    ImGui::PopStyleVar();
    ImGui::PopStyleVar();

    // DockSpace
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
    ImGui::DockSpace(ImGui::GetID("MyDockSpace"), ImVec2(0.0f, 0.0f), dockspace_flags);

    drawMenu();
    drawMenuBar();

    m_nodeWindow.drawWindow();
    m_moduleWindow.drawWindow();
    m_propertiesWindow.drawWindow();
    m_nodeGroupWindow.drawWindow();
    m_moduleBuilderWindow.drawWindow();

    ImGui::End();
}

void MainWindow::drawMenu() 
{
    if(ImGui::BeginMenuBar()) {
        if(ImGui::BeginMenu("File")) {
            if(ImGui::MenuItem("Exit"))                     
                glfwSetWindowShouldClose(m_window, 1);
            ImGui::EndMenu();
        }

        if(ImGui::BeginMenu("View")) {
            if(ImGui::MenuItem("Open Module Window"))       
                m_moduleWindow.setOpenedState(true);
            if(ImGui::MenuItem("Open Node Window"))         
                m_nodeWindow.setOpenedState(true);
            if(ImGui::MenuItem("Open Properties Window"))   
                m_propertiesWindow.setOpenedState(true);
            ImGui::EndMenu();
        }

        if(ImGui::BeginMenu("Layout")) {
            if(ImGui::MenuItem("Pinout"))                  
                m_ini_to_load = "imguiPinLayout.ini";
            if(ImGui::MenuItem("Node"))                     
                m_ini_to_load = "imguiNodeLayout.ini";
            ImGui::EndMenu();
        }

        if(ImGui::BeginMenu("Tools")) {
            if(ImGui::MenuItem("Module Builder"))
                m_moduleBuilderWindow.setOpenedState(true);
            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }

}

void MainWindow::setDarkThemeColors()
{
    auto& colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_WindowBg] = ImVec4{0.1f, 0.105f, 0.11f, 1.0f};

    // Headers
    colors[ImGuiCol_Header] = ImVec4{0.2f, 0.205f, 0.21f, 1.0f};
    colors[ImGuiCol_HeaderHovered] = ImVec4{0.3f, 0.305f, 0.31f, 1.0f};
    colors[ImGuiCol_HeaderActive] = ImVec4{0.15f, 0.1505f, 0.151f, 1.0f};

    // Buttons
    colors[ImGuiCol_Button] = ImVec4{0.2f, 0.205f, 0.21f, 1.0f};
    colors[ImGuiCol_ButtonHovered] = ImVec4{0.3f, 0.305f, 0.31f, 1.0f};
    colors[ImGuiCol_ButtonActive] = ImVec4{0.15f, 0.1505f, 0.151f, 1.0f};

    // Frame BG
    colors[ImGuiCol_FrameBg] = ImVec4{0.2f, 0.205f, 0.21f, 1.0f};
    colors[ImGuiCol_FrameBgHovered] = ImVec4{0.3f, 0.305f, 0.31f, 1.0f};
    colors[ImGuiCol_FrameBgActive] = ImVec4{0.15f, 0.1505f, 0.151f, 1.0f};

    // Tabs
    colors[ImGuiCol_Tab] = ImVec4{0.15f, 0.1505f, 0.151f, 1.0f};
    colors[ImGuiCol_TabHovered] = ImVec4{0.38f, 0.3805f, 0.381f, 1.0f};
    colors[ImGuiCol_TabActive] = ImVec4{0.28f, 0.2805f, 0.281f, 1.0f};
    colors[ImGuiCol_TabUnfocused] = ImVec4{0.15f, 0.1505f, 0.151f, 1.0f};
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4{0.2f, 0.205f, 0.21f, 1.0f};

    // Title
    colors[ImGuiCol_TitleBg] = ImVec4{0.15f, 0.1505f, 0.151f, 1.0f};
    colors[ImGuiCol_TitleBgActive] = ImVec4{0.15f, 0.1505f, 0.151f, 1.0f};
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4{0.15f, 0.1505f, 0.151f, 1.0f};
}

void MainWindow::drawMenuBar()
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 2));
    //ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 0));
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.305f, 0.31f, 0.5f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.15f, 0.1505f, 0.151f, 0.5f));

    ImGui::Begin("##Menu Bar", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

    ImGui::PopStyleColor(3);
    ImGui::Button("sfhsdfs");

    ImGui::PopStyleVar(1);
    ImGui::End();
}
