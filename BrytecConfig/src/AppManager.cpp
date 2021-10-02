#include "AppManager.h"

#include "utils/ConfigSerializer.h"
#include <iostream>

AppManager* AppManager::s_context;

AppManager::AppManager(GLFWwindow* window)
{
	s_context = this;
	m_window = window;
	newConfig();
	m_mainWindow.setWindow(window);
	m_mainWindow.setupFonts();
	m_mainWindow.setupStyle();
}

void AppManager::update()
{
	m_mainWindow.drawWindow();
}

void AppManager::newConfig()
{
	m_config = std::make_shared<Config>("");
}

void AppManager::openConfig()
{
	// Check to save opened config

	// Open dialog
	std::filesystem::path path = "";

	std::shared_ptr<Config> config = std::make_shared<Config>(path);
	ConfigSerializer serializer(config);
	if(serializer.DeserializeText(config->getFilepath()))
		m_config = config;
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
	bool fileSelected = false;// Open dialog

	if(fileSelected) {
		std::filesystem::path path;
		save(m_config, path);
		m_config->setFilepath(path);
	}

}

void AppManager::exit()
{
	// TODO check if needs to save
	glfwSetWindowShouldClose(m_window, 1);
}
