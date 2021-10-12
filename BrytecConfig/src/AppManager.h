#pragma once

#include "data/Config.h"
#include "data/Selectable.h"
#include <memory>
#include <GLFW/glfw3.h>
#include "gui/MainWindow.h"
#include "imgui.h"

class AppManager
{

public:
	AppManager(GLFWwindow* window);
	void update();
	
	std::shared_ptr<Config>& getConfig() { return m_config; }

	std::weak_ptr<Selectable> getSelectedItem() { return m_selectedItem; }
	void setSelected(std::weak_ptr<Selectable> sel) { m_selectedItem = sel; }
	void clearSelected() { setSelected(std::weak_ptr<Selectable>()); }

	void setBigIconFont(ImFont* font) { m_bigIcons = font; }
	ImFont* getBigIconFont() { return m_bigIcons; }

	// Commands
	void newConfig();
	void openConfig();
	void saveConfig();
	void saveAsConfig();
	void exit();

	static AppManager* get() { return s_context; }

private:
	void updateWindowTitle();
	void handleKeyEvents();

private:
	static AppManager* s_context;
	std::shared_ptr<Config> m_config;
	std::weak_ptr<Selectable> m_selectedItem;
	GLFWwindow* m_window;
	MainWindow m_mainWindow;
	ImFont* m_bigIcons = nullptr;

};

