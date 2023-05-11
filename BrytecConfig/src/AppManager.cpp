#include "AppManager.h"

#include "BrytecConfigEmbedded/Deserializer/BinaryPathDeserializer.h"
#include "BrytecConfigEmbedded/EBrytecApp.h"
#include "gui/NotificationWindow.h"
#include "utils/BinarySerializer.h"
#include "utils/ConfigSerializer.h"
#include "utils/FileDialogs.h"
#include "utils/ModuleSerializer.h"
#include <fstream>
#include <imgui.h>
#include <iostream>
#include <memory>
#include <yaml-cpp/yaml.h>

namespace Brytec {

namespace AppManager {

struct AppManagerData {
    std::shared_ptr<Config> config;
    std::weak_ptr<Selectable> SelectedItem;
    GLFWwindow* GLFWWindow;
    std::unique_ptr<MainWindow> mainWindow;
    int fontSize = 16;
    ImFont* BigIcons = nullptr;
    Version version;
    IOTypes::Types dragType = IOTypes::Types::Output_Batt;
    std::filesystem::path configsPath = std::filesystem::current_path();
    std::filesystem::path modulesPath = std::filesystem::current_path();
    std::filesystem::path nodeGroupsPath = std::filesystem::current_path();
    UsbManager usbManager;
    bool openLastFile = true;
    std::filesystem::path lastFile = "";
};

static AppManagerData s_data;

void init(GLFWwindow* window)
{
    s_data.mainWindow = std::make_unique<MainWindow>();
    s_data.GLFWWindow = window;
    glfwSetWindowCloseCallback(window, [](GLFWwindow* window) { AppManager::exit(); });
    s_data.mainWindow->setWindow(window);
    s_data.mainWindow->setupFonts(s_data.fontSize, s_data.fontSize + 4);
    s_data.mainWindow->setupStyle();
    loadSettings();

    if (s_data.openLastFile && std::filesystem::exists(s_data.lastFile))
        openConfigFile(s_data.lastFile);
    else
        newConfig();
}

void loadSettings()
{
    if (std::filesystem::exists("BtSettings.yaml")) {
        YAML::Node node = YAML::LoadFile("BtSettings.yaml");

        if (node["Zoom"])
            s_data.fontSize = node["Zoom"].as<int>();

        if (node["Config Path"])
            s_data.configsPath = node["Config Path"].as<std::string>();

        if (node["Module Path"])
            s_data.modulesPath = node["Module Path"].as<std::string>();

        if (node["Node Groups Path"])
            s_data.nodeGroupsPath = node["Node Groups Path"].as<std::string>();

        if (node["Open Last File"])
            s_data.openLastFile = node["Open Last File"].as<bool>();

        if (node["Last File"])
            s_data.lastFile = node["Last File"].as<std::string>();
    }
}

void saveSettings()
{
    YAML::Emitter out;
    out << YAML::BeginMap;

    out << YAML::Key << "Zoom";
    out << YAML::Value << s_data.fontSize;

    out << YAML::Key << "Config Path";
    out << YAML::Value << s_data.configsPath.string();

    out << YAML::Key << "Module Path";
    out << YAML::Value << s_data.modulesPath.string();

    out << YAML::Key << "Node Groups Path";
    out << YAML::Value << s_data.nodeGroupsPath.string();

    out << YAML::Key << "Open Last File";
    out << YAML::Value << s_data.openLastFile;

    out << YAML::Key << "Last File";
    out << YAML::Value << s_data.lastFile.string();

    out << YAML::EndMap;

    std::ofstream fout("BtSettings.yaml");
    fout << out.c_str();
}

void preFrame()
{
    s_data.mainWindow->setupFonts(s_data.fontSize, s_data.fontSize + 4);
}

void update()
{
    s_data.mainWindow->drawWindow();
    handleKeyEvents();
    s_data.usbManager.update();
}

Version getVersion()
{
    return s_data.version;
}

std::shared_ptr<Config>& getConfig()
{
    return s_data.config;
}

UsbManager& getUsbManager()
{
    return s_data.usbManager;
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

void setDragType(IOTypes::Types type)
{
    s_data.dragType = type;
}

IOTypes::Types getDragType()
{
    return s_data.dragType;
}

void clearDragType()
{
    s_data.dragType = IOTypes::Types::Undefined;
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
    auto path = FileDialogs::OpenFile("btconfig", s_data.configsPath);
    if (path.empty())
        return;

    openConfigFile(path);
}

void openConfigFile(std::filesystem::path& path)
{
    std::shared_ptr<Config> config = std::make_shared<Config>(path);
    ConfigSerializer serializer(config);

    BinaryPathDeserializer des(path);
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

    setLastFile(config->getFilepath());
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
    auto path = FileDialogs::SaveFile("btconfig", s_data.configsPath);

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

void zoom(bool plus)
{
    if (plus)
        setZoom(s_data.fontSize + 2);
    else
        setZoom(s_data.fontSize - 2);
}

int getZoom()
{
    return s_data.fontSize;
}

void setZoom(int zoom)
{
    zoom = std::clamp(zoom, 8, 26);
    s_data.fontSize = zoom;
    saveSettings();
}

const std::filesystem::path& getConfigsPath()
{
    return s_data.configsPath;
}

void setConfigsPath(const std::filesystem::path& path)
{
    s_data.configsPath = path;
    saveSettings();
}

const std::filesystem::path& getModulesPath()
{
    return s_data.modulesPath;
}

void setModulesPath(const std::filesystem::path& path)
{
    s_data.modulesPath = path;
    saveSettings();
}

const std::filesystem::path& getNodeGroupsPath()
{
    return s_data.nodeGroupsPath;
}

void setNodeGroupsPath(const std::filesystem::path& path)
{
    s_data.nodeGroupsPath = path;
    saveSettings();
}

bool getOpenLastFile()
{
    return s_data.openLastFile;
}

void setOpenLastFile(bool value)
{
    s_data.openLastFile = value;
    saveSettings();
}

const std::filesystem::path& getLastFile()
{
    return s_data.lastFile;
}

void setLastFile(const std::filesystem::path& file)
{
    s_data.lastFile = file;
    saveSettings();
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

    // Zoom
    if (control && ImGui::IsKeyPressed(GLFW_KEY_EQUAL, false))
        zoom(true);
    if (control && ImGui::IsKeyPressed(GLFW_KEY_MINUS, false))
        zoom(false);
}

}
}