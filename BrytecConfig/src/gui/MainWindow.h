#pragma once

#include <GLFW/glfw3.h>
#include "ModuleWindow.h"
#include "NodeWindow.h"
#include "PropertiesWindow.h"
#include "NodeGroupWindow.h"
#include "ModuleBuilderWindow.h"

class MainWindow {

	GLFWwindow* m_window = nullptr;
	ModuleWindow m_moduleWindow;
	NodeWindow m_nodeWindow;
	PropertiesWindow m_propertiesWindow;
	NodeGroupWindow m_nodeGroupWindow;
	ModuleBuilderWindow m_moduleBuilderWindow;
	const char* m_ini_to_load = NULL;

public:
	MainWindow();
	void setWindow(GLFWwindow* window) { m_window = window; }
	void setupFonts();
	void setupStyle();
	void loadLayout();
	void drawWindow();
	bool isNodeWindowFocused() { return m_nodeWindow.getIsWindowFocused(); }
	void removeNodeGroupContext(std::shared_ptr<NodeGroup>& nodeGroup);

private:
	void drawMenu();
	void setDarkThemeColors();
	void drawMenuBar();

};