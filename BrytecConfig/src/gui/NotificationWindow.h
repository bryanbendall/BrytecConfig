#pragma once

// Loosely based on
// imgui-notify by patrickcjk
// https://github.com/patrickcjk/imgui-notify

#include <IconsFontAwesome5.h>
#include <imgui.h>
#include <string>

enum class NotificationType {
    None,
    Success,
    Warning,
    Error,
    Info
};

struct Notification {
    std::string text = "";
    NotificationType type = NotificationType::None;
    float timeout = 3.0f;
};

class NotificationWindow {

public:
    static void drawWindow();
    static void add(const Notification& notification);

private:
    static constexpr float s_padding = 20.0f;
    static constexpr float s_spacing = 10.0f;
};