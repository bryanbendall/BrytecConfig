#pragma once

#include "ModuleWindow.h"
//#include "NodeWindow.h"
#include "PropertiesWindow.h"
#include <GLFW/glfw3.h>

class MainWindow {

	GLFWwindow* m_window;
	ModuleWindow m_moduleWindow;
	//NodeWindow m_nodeWindow;
	PropertiesWindow m_propertiesWindow;
	const char* m_ini_to_load = NULL;

public:
	MainWindow(GLFWwindow* window);
	void setupFonts();
	void setupStyle();
	void loadLayout();
	void drawWindow();

private:
	void drawMenu();
	void drawMenuBar();

};