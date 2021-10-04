#include "AppManager.h"

#include "utils/ConfigSerializer.h"
#include <iostream>
#include "utils/FileDialogs.h"

AppManager* AppManager::s_context;

AppManager::AppManager(GLFWwindow* window)
{
	s_context = this;
	m_window = window;
	glfwSetWindowCloseCallback(window, [](GLFWwindow* window) { AppManager::get()->exit(); });
	newConfig();
	m_mainWindow.setWindow(window);
	m_mainWindow.setupFonts();
	m_mainWindow.setupStyle();
}

void AppManager::update()
{
	m_mainWindow.drawWindow();
	handleKeyEvents();
}

void AppManager::newConfig()
{
	m_config = std::make_shared<Config>("");
	updateWindowTitle();
}

void AppManager::openConfig()
{
	auto path = FileDialogs::OpenFile("btconfig");
	if(path.empty())
		return;

	std::shared_ptr<Config> config = std::make_shared<Config>(path);
	ConfigSerializer serializer(config);
	if(serializer.DeserializeText(config->getFilepath())) {
		m_config = config;
		updateWindowTitle();
	}
	else
		std::cout << "Could not deserialize file: " << "" << std::endl;
}

static void save(std::shared_ptr<Config>& config, const std::filesystem::path& path)
{
	ConfigSerializer serializer(config);
	serializer.SerializeText(path);
}

void AppManager::saveConfig()
{
	if(m_config->getFilepath().empty())
		saveAsConfig();
	else
		save(m_config, m_config->getFilepath());
}

void AppManager::saveAsConfig()
{
	auto path = FileDialogs::SaveFile("btconfig");

	if(path.empty())
		return;

	save(m_config, path);
	m_config->setFilepath(path);
	
	updateWindowTitle();
}

void AppManager::exit()
{
	// TODO
	// Check should close
	bool shouldClose = true;
	if(shouldClose)
		glfwSetWindowShouldClose(m_window, GL_TRUE);
	else
		glfwSetWindowShouldClose(m_window, GL_FALSE);
}

void AppManager::updateWindowTitle()
{
	std::string title = "Brytec Config - ";

	if(!m_config->getFilepath().empty())
		title.append(m_config->getFilepath().stem().string());
	else 
		title.append("Untitled");

	glfwSetWindowTitle(m_window, title.c_str());
}

void AppManager::handleKeyEvents()
{
	bool control = ImGui::IsKeyDown(GLFW_KEY_LEFT_CONTROL) || ImGui::IsKeyDown(GLFW_KEY_RIGHT_CONTROL);
	bool shift = ImGui::IsKeyDown(GLFW_KEY_LEFT_SHIFT) || ImGui::IsKeyDown(GLFW_KEY_RIGHT_SHIFT);

	// New
	if(control && ImGui::IsKeyPressed(GLFW_KEY_N, false))
		newConfig();

	// Open
	if(control && ImGui::IsKeyPressed(GLFW_KEY_O, false))
		openConfig();

	// Save
	if(control && ImGui::IsKeyPressed(GLFW_KEY_S, false)) {
		if(shift)
			saveAsConfig();
		else
			saveConfig();
	}

}
