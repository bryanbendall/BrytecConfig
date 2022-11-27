#include "NotificationWindow.h"

#include <algorithm>
#include <iostream>
#include <vector>

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

    const auto vp_size = ImGui::GetMainViewport()->Size;

    float accumulatedHeight = 0.0f;

    ImGuiWindowFlags flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDecoration
        | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoFocusOnAppearing;

    for (int i = 0; i < s_notifications.size(); i++) {

        Notification& notification = s_notifications[i];

        ImVec4 textCol = ImGui::GetStyleColorVec4(ImGuiCol_Text);
        ImVec4 iconCol;
        std::string iconText = "";
        switch (notification.type) {
        case NotificationType::Success:
            iconCol = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
            iconText = ICON_FA_CHECK_CIRCLE;
            break;
        case NotificationType::Warning:
            iconCol = ImVec4(1.0f, 1.0f, 0.0f, 1.0f);
            iconText = ICON_FA_EXCLAMATION_TRIANGLE;
            break;
        case NotificationType::Error:
            iconCol = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
            iconText = ICON_FA_TIMES_CIRCLE;
            break;
        case NotificationType::Info:
            iconCol = ImVec4(0.0f, 0.5f, 1.0f, 1.0f);
            iconText = ICON_FA_INFO_CIRCLE;
            break;
        }

        ImGui::SetNextWindowPos(ImVec2(vp_size.x - s_padding, vp_size.y - s_padding - accumulatedHeight), ImGuiCond_Always, ImVec2(1.0f, 1.0f));
        if (notification.timeout < 0.5f) {
            ImGui::SetNextWindowBgAlpha(notification.timeout * 2.0f);
            textCol.w = notification.timeout * 2.0f;
            iconCol.w = notification.timeout * 2.0f;
        }

        char window_name[50];
        sprintf(window_name, "##Notification%d", i);

        // Round borders
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 5.f);

        ImGui::Begin(window_name, NULL, flags);

        // Wrap the text after 1/3 of the screen width
        ImGui::PushTextWrapPos(vp_size.x / 3.0f);

        if (iconText != "") {
            ImGui::TextColored(iconCol, "%s", iconText.c_str());
            ImGui::SameLine();
        }

        ImGui::TextColored(textCol, "%s", notification.text.c_str());

        ImGui::PopTextWrapPos();

        accumulatedHeight += ImGui::GetWindowHeight() + s_spacing;

        ImGui::End();

        ImGui::PopStyleVar();
    }
}

void NotificationWindow::add(const Notification& notification)
{
    s_notifications.push_back(notification);
}