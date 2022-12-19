#pragma once

#include <imgui.h>

namespace Colors {

static constexpr ImU32 PrimarySelection = IM_COL32(255, 255, 255, 255);
static constexpr ImU32 SecondarySelection = IM_COL32(255, 255, 100, 255);

namespace Module {
    static constexpr ImU32 Background = IM_COL32(50, 50, 50, 255);
    static constexpr ImU32 BackgroundHovered = IM_COL32(75, 75, 75, 255);
    static constexpr ImU32 Outline = IM_COL32(100, 100, 100, 255);
    static constexpr ImU32 OutlineSelected = IM_COL32(100, 100, 100, 255);
    static constexpr ImU32 TitleBar = IM_COL32(41, 74, 122, 255);
    static constexpr ImU32 TitleBarHovered = IM_COL32(66, 150, 250, 255);
}

namespace Node {
    static constexpr ImU32 NodeBackground = IM_COL32(50, 50, 50, 230);
    static constexpr ImU32 TitleBarSelected = IM_COL32(66, 150, 250, 230);
    static constexpr ImU32 TitleBarHovered = IM_COL32(53, 118, 200, 230);
    static constexpr ImU32 Error = IM_COL32(255, 25, 25, 204);
}

namespace NodeConnections {
    static constexpr ImU32 Boolean = IM_COL32(75, 120, 200, 255); // blue
    static constexpr ImU32 ZeroToOne = IM_COL32(200, 200, 36, 255); // yellow
    static constexpr ImU32 AnyValue = IM_COL32(99, 200, 99, 255); // green
    static constexpr ImU32 Gray = IM_COL32(162, 163, 162, 255); // gray
    static constexpr ImU32 LinkOn = IM_COL32(20, 200, 20, 255); // green
}

namespace Notifications {
    static constexpr ImU32 Success = IM_COL32(99, 200, 99, 255);
    static constexpr ImU32 Warning = IM_COL32(200, 200, 36, 255);
    static constexpr ImU32 Error = IM_COL32(255, 25, 25, 204);
    static constexpr ImU32 Info = IM_COL32(75, 120, 200, 255);
}
}