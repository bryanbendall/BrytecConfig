#include "AppManager.h"

#include "BrytecConfigEmbedded/Deserializer/BinaryDeserializer.h"
#include "BrytecConfigEmbedded/EBrytecApp.h"
#include "gui/NotificationWindow.h"
#include "utils/BinarySerializer.h"
#include "utils/ConfigSerializer.h"
#include "utils/DefaultPaths.h"
#include "utils/FileDialogs.h"
#include "utils/ModuleSerializer.h"
#include <fstream>
#include <imgui.h>
#include <iostream>
#include <memory>

namespace AppManager {

struct AppManagerData {
    std::shared_ptr<Config> config;
    std::weak_ptr<Selectable> SelectedItem;
    GLFWwindow* GLFWWindow;
    std::unique_ptr<MainWindow> mainWindow;
    ImFont* BigIcons = nullptr;
    Version version;
};

static AppManagerData s_data;

void init(GLFWwindow* window)
{
    s_data.mainWindow = std::make_unique<MainWindow>();
    s_data.GLFWWindow = window;
    glfwSetWindowCloseCallback(window, [](GLFWwindow* window) { AppManager::exit(); });
    s_data.mainWindow->setWindow(window);
    s_data.mainWindow->setupFonts();
    s_data.mainWindow->setupStyle();
    newConfig();
}

void update()
{
    s_data.mainWindow->drawWindow();
    handleKeyEvents();
}

Version getVersion()
{
    return s_data.version;
}

std::shared_ptr<Config>& getConfig()
{
    return s_data.config;
}

std::weak_ptr<Selectable> getSelectedItem()
{
    return s_data.SelectedItem;
}

void setSelected(std::weak_ptr<Selectable> sel)
{
    s_data.SelectedItem = sel;
}

void clearSelected()
{
    setSelected(std::weak_ptr<Selectable>());
}

void setBigIconFont(ImFont* font)
{
    s_data.BigIcons = font;
}

ImFont* getBigIconFont()
{
    return s_data.BigIcons;
}

void newConfig()
{
    clearSelected();
    s_data.config = std::make_shared<Config>("");
    updateWindowTitle();
    NotificationWindow::add({ "New Configuration", NotificationType::Info });
}

void openConfig()
{
    auto path = FileDialogs::OpenFile("btconfig", CONFIGS_PATH);
    if (path.empty())
        return;

    std::shared_ptr<Config> config = std::make_shared<Config>(path);
    ConfigSerializer serializer(config);

    BinaryDeserializer des;
    des.setDataFromPath(path);
    if (serializer.deserializeBinary(des)) {
        clearSelected();
        s_data.config = config;
        updateWindowTitle();
        NotificationWindow::add({ "Loaded file - " + path.filename().string(), NotificationType::Success });
    } else {
        NotificationWindow::add({ "Failed to load file - " + path.filename().string(), NotificationType::Error });
    }
}

static void save(std::shared_ptr<Config>& config)
{
    ConfigSerializer serializer(config);
    auto configBinary = serializer.serializeBinary();
    configBinary.writeToFile(config->getFilepath());

    NotificationWindow::add({ "Saved - " + config->getFilepath().filename().string(), NotificationType::Success });
}

void saveConfig()
{
    if (s_data.config->getFilepath().empty())
        saveAsConfig();
    else
        save(s_data.config);
}

void saveAsConfig()
{
    auto path = FileDialogs::SaveFile("btconfig", CONFIGS_PATH);

    if (path.empty())
        return;

    if (path.extension().empty())
        path.replace_extension("btconfig");

    s_data.config->setFilepath(path);
    save(s_data.config);

    updateWindowTitle();
}

void exit()
{
    // TODO
    // Check should close
    bool shouldClose = true;
    if (shouldClose)
        glfwSetWindowShouldClose(s_data.GLFWWindow, GL_TRUE);
    else
        glfwSetWindowShouldClose(s_data.GLFWWindow, GL_FALSE);
}

void updateWindowTitle()
{
    std::string title = "Brytec Config - ";

    if (!s_data.config->getFilepath().empty())
        title.append(s_data.config->getName());
    else
        title.append("Untitled");

    glfwSetWindowTitle(s_data.GLFWWindow, title.c_str());
}

void handleKeyEvents()
{
    bool control = ImGui::IsKeyDown(GLFW_KEY_LEFT_CONTROL) || ImGui::IsKeyDown(GLFW_KEY_RIGHT_CONTROL);
    bool shift = ImGui::IsKeyDown(GLFW_KEY_LEFT_SHIFT) || ImGui::IsKeyDown(GLFW_KEY_RIGHT_SHIFT);

    // New
    if (control && ImGui::IsKeyPressed(GLFW_KEY_N, false))
        newConfig();

    // Open
    if (control && ImGui::IsKeyPressed(GLFW_KEY_O, false))
        openConfig();

    // Save
    if (control && ImGui::IsKeyPressed(GLFW_KEY_S, false)) {
        if (shift)
            saveAsConfig();
        else
            saveConfig();
    }

    // Delete
    if (ImGui::IsKeyPressed(GLFW_KEY_DELETE, false)) {
        if (!s_data.mainWindow->isNodeWindowFocused()) {
            std::shared_ptr<Selectable> selected = s_data.SelectedItem.lock();
            if (!selected)
                return;

            if (auto module = std::dynamic_pointer_cast<Module>(selected)) {
                clearSelected();
                s_data.config->removeModule(module);
            }

            if (auto nodeGroup = std::dynamic_pointer_cast<NodeGroup>(selected)) {
                clearSelected();
                s_data.config->removeNodeGroup(nodeGroup);
                s_data.mainWindow->removeNodeGroupContext(nodeGroup);
            }
        }
    }
}

}