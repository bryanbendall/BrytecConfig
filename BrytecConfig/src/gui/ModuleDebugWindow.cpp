#include "ModuleDebugWindow.h"

#include "communication/ModuleDebug.h"
#include <IconsFontAwesome5.h>
#include <imgui.h>

namespace Brytec {

ImVector<char*> Items;

ModuleDebugWindow::ModuleDebugWindow()
{
}

void ModuleDebugWindow::drawWindow()
{
    if (m_openOnMessage && ModuleDebug::hasNewData())
        m_opened = true;

    if (!m_opened)
        return;

    ImGui::Begin(ICON_FA_BUG " Module Debug", &m_opened, ImGuiWindowFlags_MenuBar);

    drawMenubar();

    static bool AutoScroll = true;
    static bool ScrollToBottom = false;

    if (ImGui::BeginChild("ScrollingRegion", ImVec2(0, 0), ImGuiChildFlags_None, ImGuiWindowFlags_HorizontalScrollbar)) {

        ImGuiTextBuffer& log = ModuleDebug::getText();
        ImGui::TextUnformatted(log.begin(), log.end());

        // Keep up at the bottom of the scroll region if we were already at the bottom at the beginning of the frame.
        // Using a scrollbar or mouse-wheel will take away from the bottom edge.
        if (ScrollToBottom || (AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()))
            ImGui::SetScrollHereY(1.0f);
        ScrollToBottom = false;
    }
    ImGui::EndChild();

    ImGui::End();
}

void ModuleDebugWindow::drawMenubar()
{
    if (ImGui::BeginMenuBar()) {

        if (ImGui::MenuItem(ICON_FA_TIMES "  Clear"))
            ModuleDebug::getText().clear();

        ImGui::TextDisabled("|");

        ImGui::Checkbox("Open on new message", &m_openOnMessage);

        ImGui::EndMenuBar();
    }
}
}