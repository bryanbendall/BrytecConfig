#include "MainWindow.h"

#include <imgui.h>
//#include <misc/freetype/imgui_freetype.h>
#include <backends/imgui_impl_opengl3.h>
#include <IconsFontAwesome5.h>
#include "AppManager.h"
#include <functional>
#include "utils/DefaultPaths.h"

MainWindow::MainWindow() 
{ 
}

void MainWindow::setupFonts() 
{
    // Load Fonts
    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->AddFontFromFileTTF(FONTS_PATH_DROID_SANS, 16.0f);

    static const ImWchar icons_ranges[] = {ICON_MIN_FA, ICON_MAX_FA, 0};
    ImFontConfig icons_config;
    icons_config.MergeMode = true;
    icons_config.PixelSnapH = true;
    io.Fonts->AddFontFromFileTTF(FONTS_PATH_FONT_AWESOME, 14.0f, &icons_config, icons_ranges);
    //ImGuiFreeType::BuildFontAtlas(io.Fonts, ImGuiFreeType::ForceAutoHint | ImGuiFreeType::MonoHinting);

    ImFontConfig icons_config2;
    icons_config2.PixelSnapH = true;
    AppManager::setBigIconFont(io.Fonts->AddFontFromFileTTF(FONTS_PATH_FONT_AWESOME, 20.0f, &icons_config2, icons_ranges));

    ImGui_ImplOpenGL3_DestroyDeviceObjects();
    ImGui_ImplOpenGL3_CreateDeviceObjects();
}

void MainWindow::setupStyle() 
{
    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    ImGuiStyle* style = &ImGui::GetStyle();
    style->Colors[ImGuiCol_TitleBgActive] = style->Colors[ImGuiCol_TitleBg];

    // Combo boxes
    style->Colors[ImGuiCol_Button] = style->Colors[ImGuiCol_FrameBg];
    style->Colors[ImGuiCol_ButtonHovered] = style->Colors[ImGuiCol_FrameBgHovered];

    style->FrameRounding = 3.0f;
    style->FramePadding = ImVec2(4.0f, 3.0f);

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
    // Note: Switch this to true to enable dockspace
    static bool dockspaceOpen = true;
    static bool opt_fullscreen_persistant = true;
    bool opt_fullscreen = opt_fullscreen_persistant;
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

    // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
    // because it would be confusing to have two docking targets within each others.
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    if(opt_fullscreen) {
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
}

    // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background and handle the pass-thru hole, so we ask Begin() to not render a background.
    if(dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
        window_flags |= ImGuiWindowFlags_NoBackground;

    // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
    // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive, 
    // all active windows docked into it will lose their parent and become undocked.
    // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise 
    // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
    ImGui::PopStyleVar();

    if(opt_fullscreen)
        ImGui::PopStyleVar(2);

    // DockSpace
    ImGuiIO& io = ImGui::GetIO();
    ImGuiStyle& style = ImGui::GetStyle();
    if(io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
    }

    drawMenu();
    drawMenuBar();

    ImGui::ShowDemoWindow();
    ImGui::ShowMetricsWindow();

    m_nodeWindow.drawWindow();
    m_moduleWindow.drawWindow();
    m_propertiesWindow.drawWindow();
    m_nodeGroupWindow.drawWindow();
    m_moduleBuilderWindow.drawWindow();

    ImGui::End();

}

void MainWindow::removeNodeGroupContext(std::shared_ptr<NodeGroup>& nodeGroup)
{
    m_nodeWindow.removeContext(nodeGroup);
}

void MainWindow::drawMenu() 
{
    if(ImGui::BeginMenuBar()) {
        if(ImGui::BeginMenu("File")) {
            if(ImGui::MenuItem("New", "Ctrl+N"))
                AppManager::newConfig();
            if(ImGui::MenuItem("Open", "Ctrl+O"))
                AppManager::openConfig();
            ImGui::Separator();
            if(ImGui::MenuItem("Save", "Ctrl+S"))
                AppManager::saveConfig();
            if(ImGui::MenuItem("Save As", "Ctrl+Shift+S"))
                AppManager::saveAsConfig();
            ImGui::Separator();
            if(ImGui::MenuItem("Exit", "Alt+F4"))
                AppManager::exit();
            ImGui::EndMenu();
        }

        if(ImGui::BeginMenu("View")) {
            if(ImGui::MenuItem("Module Window", "", m_moduleWindow.getOpenedState()))       
                m_moduleWindow.setOpenedState(true);
            if(ImGui::MenuItem("Node Window", "", m_nodeWindow.getOpenedState()))
                m_nodeWindow.setOpenedState(true);
            if(ImGui::MenuItem("Properties Window", "", m_propertiesWindow.getOpenedState()))
                m_propertiesWindow.setOpenedState(true);
            ImGui::EndMenu();
        }

#if 0
        if(ImGui::BeginMenu("Layout")) {
            if(ImGui::MenuItem("Pinout"))                  
                m_ini_to_load = "imguiPinLayout.ini";
            if(ImGui::MenuItem("Node"))                     
                m_ini_to_load = "imguiNodeLayout.ini";
            ImGui::EndMenu();
        }
#endif

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

static bool anyItemHovered = false;
static int hoveredTime = 0;

template<typename T>
static void ToolbarButton(const char* icon, const char* tooltip, T&& function)
{
    static float iconSize = 40.0f;
    ImGui::SameLine();
    if(ImGui::Button(icon, ImVec2(iconSize, iconSize)))
        function();
    ImGui::PopFont();
    ImGui::PopStyleVar(2);
    if(ImGui::IsItemHovered()) {
        anyItemHovered = true;
        if(hoveredTime > 40)
            ImGui::SetTooltip(tooltip, "");
    }
    ImGui::PushFont(AppManager::getBigIconFont());
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 2));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
}

static void ToolbarSeperator()
{
    ImGui::SameLine();
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    draw_list->AddLine(ImVec2(ImGui::GetCursorPosX() + 4.0f, ImGui::GetCursorPosY() + 28.0f), 
                        ImVec2(ImGui::GetCursorPosX() + 4.0f, ImGui::GetCursorPosY() + 55.0f), 
                        IM_COL32(80,80,80,255));
    ImGui::SameLine();
    ImGui::Dummy(ImVec2(10.0f, 0.0f));
}

void MainWindow::drawMenuBar()
{
    ImGui::PushFont(AppManager::getBigIconFont());

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 2));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.305f, 0.31f, 0.5f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.15f, 0.1505f, 0.151f, 0.5f));

    ImGui::Begin("##Menu Bar", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

    ToolbarButton(ICON_FA_FILE,         "New",  &AppManager::newConfig);
    ToolbarButton(ICON_FA_FOLDER_OPEN,  "Open", &AppManager::openConfig);
    ToolbarButton(ICON_FA_SAVE,         "Save", &AppManager::saveConfig);

    ToolbarSeperator();

    ToolbarButton(ICON_FA_CUBES,        "Module Builder", [&](){m_moduleBuilderWindow.setOpenedState(true);});

    if(anyItemHovered) {
        anyItemHovered = false;
        hoveredTime++;
    } else {
        hoveredTime = 0;
    }

    ImGui::PopStyleColor(3);
    ImGui::PopStyleVar(2);
    ImGui::End();

    ImGui::PopFont();
}
