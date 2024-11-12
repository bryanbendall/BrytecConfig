#pragma once

#include "communication/CanBusStream.h"
#include "communication/net/NetManager.h"
#include "communication/usb/UsbManager.h"
#include "data/Config.h"
#include "data/Selectable.h"
#include "gui/MainWindow.h"
#include "imgui.h"
#include <GLFW/glfw3.h>
#include <memory>

namespace serial {
class PortInfo;
}

namespace Brytec {

    struct Version {
        const uint8_t Major = 0;
        const uint8_t Minor = 1;
    };

    class AppManager {

    public:
        static void init(GLFWwindow* window);
        static void loadSettings();
        static void saveSettings();
        static void preFrame();
        static void update();

        static Version getVersion();

        static std::shared_ptr<Config>& getConfig();

        static UsbManager& getUsbManager();
        static NetManager& getNetManager();
        static CanBusStream& getCanBusStream();

        // Selection ////////////////////////////////
        static std::weak_ptr<Selectable> getSelectedItem();
        static void setSelected(std::weak_ptr<Selectable> sel);
        static void clearSelected();

        template <typename T>
        static std::shared_ptr<T> getSelected()
        {
            return std::dynamic_pointer_cast<T>(getSelectedItem().lock());
        }

        template <typename T>
        static bool isSelected(std::shared_ptr<T> other)
        {
            return getSelected<T>() == other;
        }
        /////////////////////////////////////////////

        static void setDragType(IOTypes::Types type);
        static IOTypes::Types getDragType();
        static void clearDragType();

        static void setBigIconFont(ImFont* font);
        static ImFont* getBigIconFont();

        // Commands
        static void newConfig();
        static void openConfig();
        static void openConfigFile(std::filesystem::path& path);
        static void saveConfig();
        static void saveAsConfig();
        static void exit();

        static void zoom(bool plus);
        static int getZoom();
        static void setZoom(int zoom);

        static const std::filesystem::path& getConfigsPath();
        static void setConfigsPath(const std::filesystem::path& path);
        static const std::filesystem::path& getModulesPath();
        static void setModulesPath(const std::filesystem::path& path);
        static const std::filesystem::path& getNodeGroupsPath();
        static void setNodeGroupsPath(const std::filesystem::path& path);

        static bool getOpenLastFile();
        static void setOpenLastFile(bool value);
        static const std::filesystem::path& getLastFile();
        static void setLastFile(const std::filesystem::path& file);

        static serial::PortInfo& getLastSerialPort();
        static void setLastSerialPort(serial::PortInfo& port);

        static std::string& getIpAdderess();
        static void setIpAddress(std::string& ipAddress);

        static void openBrowser(const std::string& url);
        static void openDocumentation(const std::string& page);

    private:
        static void updateWindowTitle();
        static void handleKeyEvents();

        static void save(std::shared_ptr<Config>& config);
    };
}