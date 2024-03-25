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
#include <serial/serial.h>
#include <yaml-cpp/yaml.h>

namespace YAML {
template <>
struct convert<serial::PortInfo> {
    static Node encode(const serial::PortInfo& rhs)
    {
        Node node;
        node.push_back(rhs.port);
        node.push_back(rhs.description);
        node.push_back(rhs.hardware_id);
        return node;
    }

    static bool decode(const Node& node, serial::PortInfo& rhs)
    {
        if (!node.IsSequence() || node.size() != 3) {
            return false;
        }

        rhs.port = node[0].as<std::string>();
        rhs.description = node[1].as<std::string>();
        rhs.hardware_id = node[2].as<std::string>();
        return true;
    }
};
}

YAML::Emitter& operator<<(YAML::Emitter& out, const serial::PortInfo& v)
{
    out << YAML::BeginSeq << v.port << v.description << v.hardware_id << YAML::EndSeq;
    return out;
}

namespace Brytec {

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
    CanBusStream canBusStream;
    UsbManager usbManager;
    bool openLastFile = true;
    std::filesystem::path lastFile = "";
    serial::PortInfo lastSerialPort;
};

static AppManagerData s_data;

void AppManager::init(GLFWwindow* window)
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

void AppManager::loadSettings()
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

        if (node["Last Serial Port"])
            s_data.lastSerialPort = node["Last Serial Port"].as<serial::PortInfo>();

        if (node["Module Window State"])
            s_data.mainWindow->m_moduleWindow.setOpenedState(node["Module Window State"].as<bool>());

        if (node["Node Group Window State"])
            s_data.mainWindow->m_nodeGroupWindow.setOpenedState(node["Node Group Window State"].as<bool>());

        if (node["Node Window State"])
            s_data.mainWindow->m_nodeWindow.setOpenedState(node["Node Window State"].as<bool>());

        if (node["Properties Window State"])
            s_data.mainWindow->m_propertiesWindow.setOpenedState(node["Properties Window State"].as<bool>());

        if (node["Monitor Window State"])
            s_data.mainWindow->m_monitorWindow.setOpenedState(node["Monitor Window State"].as<bool>());

        if (node["Debug Window State"])
            s_data.mainWindow->m_moduleDebugWindow.setOpenedState(node["Debug Window State"].as<bool>());
    }
}

void AppManager::saveSettings()
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

    out << YAML::Key << "Last Serial Port";
    out << YAML::Value << s_data.lastSerialPort;

    out << YAML::Key << "Module Window State";
    out << YAML::Value << s_data.mainWindow->m_moduleWindow.getOpenedState();

    out << YAML::Key << "Node Group Window State";
    out << YAML::Value << s_data.mainWindow->m_nodeGroupWindow.getOpenedState();

    out << YAML::Key << "Node Window State";
    out << YAML::Value << s_data.mainWindow->m_nodeWindow.getOpenedState();

    out << YAML::Key << "Properties Window State";
    out << YAML::Value << s_data.mainWindow->m_propertiesWindow.getOpenedState();

    out << YAML::Key << "Monitor Window State";
    out << YAML::Value << s_data.mainWindow->m_monitorWindow.getOpenedState();

    out << YAML::Key << "Debug Window State";
    out << YAML::Value << s_data.mainWindow->m_moduleDebugWindow.getOpenedState();

    out << YAML::EndMap;

    std::ofstream fout("BtSettings.yaml");
    fout << out.c_str();
}

void AppManager::preFrame()
{
    s_data.mainWindow->setupFonts(s_data.fontSize, s_data.fontSize + 4);
}

void AppManager::update()
{
    s_data.mainWindow->drawWindow();
    handleKeyEvents();
    s_data.usbManager.update();
}

Version AppManager::getVersion()
{
    return s_data.version;
}

std::shared_ptr<Config>& AppManager::getConfig()
{
    return s_data.config;
}

UsbManager& AppManager::getUsbManager()
{
    return s_data.usbManager;
}

CanBusStream& AppManager::getCanBusStream()
{
    return s_data.canBusStream;
}

std::weak_ptr<Selectable> AppManager::getSelectedItem()
{
    return s_data.SelectedItem;
}

void AppManager::setSelected(std::weak_ptr<Selectable> sel)
{
    s_data.SelectedItem = sel;
}

void AppManager::clearSelected()
{
    setSelected(std::weak_ptr<Selectable>());
}

void AppManager::setDragType(IOTypes::Types type)
{
    s_data.dragType = type;
}

IOTypes::Types AppManager::getDragType()
{
    return s_data.dragType;
}

void AppManager::clearDragType()
{
    s_data.dragType = IOTypes::Types::Undefined;
}

void AppManager::setBigIconFont(ImFont* font)
{
    s_data.BigIcons = font;
}

ImFont* AppManager::getBigIconFont()
{
    return s_data.BigIcons;
}

void AppManager::newConfig()
{
    clearSelected();
    s_data.config = std::make_shared<Config>("");
    updateWindowTitle();
    NotificationWindow::add({ "New Configuration", NotificationType::Info });
}

void AppManager::openConfig()
{
    auto path = FileDialogs::OpenFile("btconfig", s_data.configsPath);
    if (path.empty())
        return;

    openConfigFile(path);
}

void AppManager::openConfigFile(std::filesystem::path& path)
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

void AppManager::saveConfig()
{
    if (s_data.config->getFilepath().empty())
        saveAsConfig();
    else
        save(s_data.config);
}

void AppManager::saveAsConfig()
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

void AppManager::exit()
{
    saveSettings();

    // TODO
    // Check should close
    bool shouldClose = true;
    if (shouldClose)
        glfwSetWindowShouldClose(s_data.GLFWWindow, GL_TRUE);
}

void AppManager::zoom(bool plus)
{
    if (plus)
        setZoom(s_data.fontSize + 2);
    else
        setZoom(s_data.fontSize - 2);
}

int AppManager::getZoom()
{
    return s_data.fontSize;
}

void AppManager::setZoom(int zoom)
{
    zoom = std::clamp(zoom, 8, 26);
    s_data.fontSize = zoom;
}

const std::filesystem::path& AppManager::getConfigsPath()
{
    return s_data.configsPath;
}

void AppManager::setConfigsPath(const std::filesystem::path& path)
{
    s_data.configsPath = path;
}

const std::filesystem::path& AppManager::getModulesPath()
{
    return s_data.modulesPath;
}

void AppManager::setModulesPath(const std::filesystem::path& path)
{
    s_data.modulesPath = path;
}

const std::filesystem::path& AppManager::getNodeGroupsPath()
{
    return s_data.nodeGroupsPath;
}

void AppManager::setNodeGroupsPath(const std::filesystem::path& path)
{
    s_data.nodeGroupsPath = path;
}

bool AppManager::getOpenLastFile()
{
    return s_data.openLastFile;
}

void AppManager::setOpenLastFile(bool value)
{
    s_data.openLastFile = value;
}

const std::filesystem::path& AppManager::getLastFile()
{
    return s_data.lastFile;
}

void AppManager::setLastFile(const std::filesystem::path& file)
{
    s_data.lastFile = file;
}

serial::PortInfo& AppManager::getLastSerialPort()
{
    return s_data.lastSerialPort;
}

void AppManager::setLastSerialPort(serial::PortInfo& port)
{
    s_data.lastSerialPort = port;
}

void AppManager::updateWindowTitle()
{
    std::string title = "Brytec Config - ";

    if (!s_data.config->getFilepath().empty())
        title.append(s_data.config->getName());
    else
        title.append("Untitled");

    glfwSetWindowTitle(s_data.GLFWWindow, title.c_str());
}

void AppManager::handleKeyEvents()
{
    bool control = ImGui::IsKeyDown(ImGuiKey_LeftCtrl) || ImGui::IsKeyDown(ImGuiKey_RightCtrl);
    bool shift = ImGui::IsKeyDown(ImGuiKey_LeftShift) || ImGui::IsKeyDown(ImGuiKey_RightShift);

    // New
    if (control && ImGui::IsKeyPressed(ImGuiKey_N, false))
        newConfig();

    // Open
    if (control && ImGui::IsKeyPressed(ImGuiKey_O, false))
        openConfig();

    // Save
    if (control && ImGui::IsKeyPressed(ImGuiKey_S, false)) {
        if (shift)
            saveAsConfig();
        else
            saveConfig();
    }

    // Delete
    if (ImGui::IsKeyPressed(ImGuiKey_Delete, false)) {
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
    if (control && ImGui::IsKeyPressed(ImGuiKey_Equal, false))
        zoom(true);
    if (control && ImGui::IsKeyPressed(ImGuiKey_Minus, false))
        zoom(false);
}

void AppManager::save(std::shared_ptr<Config>& config)
{
    ConfigSerializer serializer(config);
    auto configBinary = serializer.serializeBinary();
    configBinary.writeToFile(config->getFilepath());

    NotificationWindow::add({ "Saved - " + config->getFilepath().filename().string(), NotificationType::Success });

    setLastFile(config->getFilepath());
}
}