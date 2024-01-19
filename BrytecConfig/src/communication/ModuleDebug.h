#pragma once

#include <imgui.h>
#include <string>

namespace Brytec {

class ModuleDebug {

public:
    static void push(const char* str, uint32_t length);
    static ImGuiTextBuffer& getText();
    static bool hasNewData();

private:
    static bool m_newData;
};
}