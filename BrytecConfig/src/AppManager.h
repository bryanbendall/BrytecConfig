#pragma once

#include "data/Config.h"
#include "data/Selectable.h"
#include <memory>
#include <GLFW/glfw3.h>
#include "gui/MainWindow.h"

class AppManager
{
	static AppManager* s_context;
	Config m_config;
	std::weak_ptr<Selectable> m_selectedItem;
	MainWindow m_mainWindow;

public:
	AppManager(GLFWwindow* window);
	void update();

	static Config& getConfig() { return s_context->m_config; }
	static std::weak_ptr<Selectable> getSelectedItem() { return s_context->m_selectedItem; }
	static void setSelected(std::weak_ptr<Selectable> sel) { s_context->m_selectedItem = sel; }

};

