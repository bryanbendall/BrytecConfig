#include "AppManager.h"

AppManager* AppManager::s_context;

AppManager::AppManager(GLFWwindow* window)
{
	s_context = this;
	m_mainWindow.setWindow(window);
	m_mainWindow.setupFonts();
	m_mainWindow.setupStyle();
}

void AppManager::update()
{
	m_mainWindow.drawWindow();
}
