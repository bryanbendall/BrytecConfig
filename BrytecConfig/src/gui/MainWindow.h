#pragma once

#include "ModuleBuilderWindow.h"
#include "ModuleWindow.h"
#include "NodeGroupWindow.h"
#include "NodeWindow.h"
#include "PropertiesWindow.h"
#include <GLFW/glfw3.h>

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
    void setDarkThemeColors();
    void drawMenuBar();

private:
    GLFWwindow* m_window = nullptr;
    ModuleWindow m_moduleWindow;
    NodeWindow m_nodeWindow;
    PropertiesWindow m_propertiesWindow;
    NodeGroupWindow m_nodeGroupWindow;
    ModuleBuilderWindow m_moduleBuilderWindow;
    const char* m_ini_to_load = NULL;
};