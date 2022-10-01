#pragma once

#include "data/Config.h"
#include "data/Selectable.h"
#include "gui/MainWindow.h"
#include "imgui.h"
#include <GLFW/glfw3.h>
#include <memory>

namespace AppManager {

struct Version {
    const uint8_t Major = 0;
    const uint8_t Minor = 1;
};

void init(GLFWwindow* window);
void update();

Version getVersion();

std::shared_ptr<Config>& getConfig();

// Selection ////////////////////////////////
std::weak_ptr<Selectable> getSelectedItem();
void setSelected(std::weak_ptr<Selectable> sel);
void clearSelected();

template <typename T>
std::shared_ptr<T> getSelected()
{
    return std::dynamic_pointer_cast<T>(getSelectedItem().lock());
}

template <typename T>
bool isSelected(std::shared_ptr<T> other)
{
    return getSelected<T>() == other;
}
/////////////////////////////////////////////

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
