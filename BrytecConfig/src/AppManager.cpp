#include "AppManager.h"

#include "BrytecConfigEmbedded/Utils/BinaryDeserializer.h"
#include "utils/BinarySerializer.h"
#include "utils/ConfigSerializer.h"
#include "utils/DefaultPaths.h"
#include "utils/FileDialogs.h"
#include <fstream>
#include <imgui.h>
#include <iostream>
#include <memory>

namespace AppManager {

static Data data;

void init(GLFWwindow* window)
{
    data.mainWindow = std::make_unique<MainWindow>();
    data.GLFWWindow = window;
    glfwSetWindowCloseCallback(window, [](GLFWwindow* window) { AppManager::exit(); });
    data.mainWindow->setWindow(window);
    data.mainWindow->setupFonts();
    data.mainWindow->setupStyle();
    newConfig();
}

void update()
{
    data.mainWindow->drawWindow();
    handleKeyEvents();
}

Version getVersion()
{
    return data.version;
}

std::shared_ptr<Config>&
getConfig()
{
    return data.config;
}

std::weak_ptr<Selectable> getSelectedItem()
{
    return data.SelectedItem;
}

void setSelected(std::weak_ptr<Selectable> sel)
{
    data.SelectedItem = sel;
}

void clearSelected()
{
    setSelected(std::weak_ptr<Selectable>());
}

void setBigIconFont(ImFont* font)
{
    data.BigIcons = font;
}

ImFont* getBigIconFont()
{
    return data.BigIcons;
}

void newConfig()
{
    clearSelected();
    data.config = std::make_shared<Config>("");
    updateWindowTitle();
}

void openConfig()
{
    auto path = FileDialogs::OpenFile("btconfig", CONFIGS_PATH);
    if (path.empty())
        return;

    std::shared_ptr<Config> config = std::make_shared<Config>(path);
    ConfigSerializer serializer(config);

    ///////////////////////////////
    // Test ///////////////////////
    BinaryDeserializer des(path);
    if (serializer.deserializeBinary(des)) {
        clearSelected();
        data.config = config;
        updateWindowTitle();
    } else {
        std::cout << "Could not deserialize file: "
                  << "" << std::endl;
    }
}

static void save(std::shared_ptr<Config>& config)
{
    ConfigSerializer serializer(config);
    // serializer.serializeText(path);
    auto configBinary = serializer.serializeBinary();
    configBinary.writeToFile(config->getFilepath());
}

void saveConfig()
{
    if (data.config->getFilepath().empty())
        saveAsConfig();
    else
        save(data.config);
}

void saveAsConfig()
{
    auto path = FileDialogs::SaveFile("btconfig", CONFIGS_PATH);

    if (path.empty())
        return;

    if (path.extension().empty())
        path.replace_extension("btconfig");

    data.config->setFilepath(path);
    save(data.config);

    updateWindowTitle();
}

void exit()
{
    // TODO
    // Check should close
    bool shouldClose = true;
    if (shouldClose)
        glfwSetWindowShouldClose(data.GLFWWindow, GL_TRUE);
    else
        glfwSetWindowShouldClose(data.GLFWWindow, GL_FALSE);
}

void updateWindowTitle()
{
    std::string title = "Brytec Config - ";

    if (!data.config->getFilepath().empty())
        title.append(data.config->getName());
    else
        title.append("Untitled");

    glfwSetWindowTitle(data.GLFWWindow, title.c_str());
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
        if (!data.mainWindow->isNodeWindowFocused()) {
            std::shared_ptr<Selectable> selected = data.SelectedItem.lock();
            if (!selected)
                return;

            if (auto module = std::dynamic_pointer_cast<Module>(selected)) {
                clearSelected();
                data.config->removeModule(module);
            }

            if (auto nodeGroup = std::dynamic_pointer_cast<NodeGroup>(selected)) {
                clearSelected();
                data.config->removeNodeGroup(nodeGroup);
                data.mainWindow->removeNodeGroupContext(nodeGroup);
            }
        }
    }
}

}