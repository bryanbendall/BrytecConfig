#pragma once

#include "data/Config.h"
#include "data/Selectable.h"
#include <memory>
#include <GLFW/glfw3.h>
#include "gui/MainWindow.h"
#include "imgui.h"

namespace AppManager
{

	struct Data
	{
		std::shared_ptr<Config> Config;
		std::weak_ptr<Selectable> SelectedItem;
		GLFWwindow* GLFWWindow;
		std::unique_ptr<MainWindow> MainWindow;
		ImFont* BigIcons = nullptr;
	};

	void init(GLFWwindow* window);
	void update();
	
	std::shared_ptr<Config>& getConfig();

	std::weak_ptr<Selectable> getSelectedItem();
	void setSelected(std::weak_ptr<Selectable> sel);
	void clearSelected();

	void setBigIconFont(ImFont* font);
	ImFont* getBigIconFont();

	// Commands
	void newConfig();
	void openConfig();
	void saveConfig();
	void saveAsConfig();
	void exit();

	void updateWindowTitle();
	void handleKeyEvents();

};

