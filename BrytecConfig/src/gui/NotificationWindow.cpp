#include "NotificationWindow.h"

#include "utils/Colors.h"
#include <algorithm>
#include <iostream>
#include <vector>

namespace Brytec {

static std::vector<Notification> s_notifications;

void NotificationWindow::drawWindow()
{
    float deltaTime = ImGui::GetIO().DeltaTime;

    // Hack for when a file dialog is open
    if (deltaTime > 0.1f)
        return;

    s_notifications.erase(
        std::remove_if(s_notifications.begin(), s_notifications.end(), [deltaTime](auto& x) {
            x.timeout -= deltaTime;
            return x.timeout <= 0.0f;
        }),
        s_notifications.end());

    const auto vpSize = ImGui::GetMainViewport()->Size;

    float accumulatedHeight = 0.0f;

    ImGuiWindowFlags flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDecoration
        | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoFocusOnAppearing;

    for (int i = 0; i < s_notifications.size(); i++) {

        Notification& notification = s_notifications[i];

        ImVec4 iconCol;
        std::string iconText = "";
        switch (notification.type) {
        case NotificationType::Success:
            iconCol = ImColor(Colors::Notifications::Success);
            iconText = ICON_FA_CHECK_CIRCLE;
            break;
        case NotificationType::Warning:
            iconCol = ImColor(Colors::Notifications::Warning);
            iconText = ICON_FA_EXCLAMATION_TRIANGLE;
            break;
        case NotificationType::Error:
            iconCol = ImColor(Colors::Notifications::Error);
            iconText = ICON_FA_TIMES_CIRCLE;
            break;
        case NotificationType::Info:
            iconCol = ImColor(Colors::Notifications::Info);
            iconText = ICON_FA_INFO_CIRCLE;
            break;
        }

        if (accumulatedHeight > vpSize.y)
            return;

        ImGui::SetNextWindowPos(ImVec2(vpSize.x - s_padding, vpSize.y - s_padding - accumulatedHeight), ImGuiCond_Always, ImVec2(1.0f, 1.0f));

        // Fade out
        float alpha = 1.0f;
        static constexpr float fadeTime = 0.1f;
        if (notification.timeout < fadeTime)
            alpha = notification.timeout / fadeTime;
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);

        // Round borders
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 5.f);

        char windowName[50];
        sprintf(windowName, "##Notification%d", i);
        ImGui::Begin(windowName, NULL, flags);

        // Wrap the text after 1/3 of the screen width
        ImGui::PushTextWrapPos(vpSize.x / 3.0f);

        if (iconText != "") {
            ImGui::TextColored(iconCol, "%s", iconText.c_str());
            ImGui::SameLine();
        }

        ImGui::Text("%s", notification.text.c_str());

        ImGui::PopTextWrapPos();

        {
            ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive));

            float progress = notification.timeout / notification.totalTime;
            ImGui::ProgressBar(progress, ImVec2(0.0f, 3.0f), "");

            ImGui::PopStyleColor();
        }

        accumulatedHeight += ImGui::GetWindowHeight() + s_spacing;

        ImGui::End();

        ImGui::PopStyleVar();
        ImGui::PopStyleVar();
    }
}

void NotificationWindow::add(const Notification& notification)
{
    s_notifications.push_back(notification);
    s_notifications.back().totalTime = s_notifications.back().timeout;
}
}