#pragma once

#define IMGUI_DEFINE_MATH_OPERATORS

#include "CommunicationModals.h"
#include "ModuleBuilderWindow.h"
#include "ModuleDebugWindow.h"
#include "ModuleWindow.h"
#include "MonitorWindow.h"
#include "NodeGroupWindow.h"
#include "NodeWindow.h"
#include "PropertiesWindow.h"
#include "SettingsWindow.h"
#include <GLFW/glfw3.h>

namespace Brytec {

class MainWindow {

public:
    MainWindow();
    void setupFonts(int size, int iconSize);
    void setupStyle();
    void loadLayout();
    void drawWindow();
    void setWindow(GLFWwindow* window) { m_window = window; }
    bool isNodeWindowFocused() { return m_nodeWindow.getIsWindowFocused(); }
    void removeNodeGroupContext(std::shared_ptr<NodeGroup>& nodeGroup);

private:
    void drawMenu();
    void drawMenuBar();

private:
    GLFWwindow* m_window = nullptr;
    ModuleWindow m_moduleWindow;
    NodeWindow m_nodeWindow;
    PropertiesWindow m_propertiesWindow;
    NodeGroupWindow m_nodeGroupWindow;
    ModuleBuilderWindow m_moduleBuilderWindow;
    SettingsWindow m_settingsWindow;
    MonitorWindow m_monitorWindow;
    ModuleDebugWindow m_moduleDebugWindow;
    CommunicationModals m_communicationModals;
    const char* m_ini_to_load = NULL;

    friend class AppManager;
};

}