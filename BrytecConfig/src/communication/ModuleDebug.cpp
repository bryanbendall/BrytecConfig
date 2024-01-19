#include "ModuleDebug.h"

namespace Brytec {

static ImGuiTextBuffer m_log;

bool ModuleDebug::m_newData = true;

void ModuleDebug::push(const char* str, uint32_t length)
{
    m_newData = true;
    m_log.append(str, str + length);
}

ImGuiTextBuffer& ModuleDebug::getText()
{
    return m_log;
}

bool ModuleDebug::hasNewData()
{
    if (m_newData) {
        m_newData = false;
        return true;
    }

    return false;
}
}