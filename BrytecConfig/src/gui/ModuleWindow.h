#pragma once

#include <vector>
#include <imgui.h>
#include <memory>
#include "data/Module.h"

class ModuleWindow
{
    ImU32 ColorStyle_Background =           IM_COL32(50, 50, 50, 255);
    ImU32 ColorStyle_BackgroundHovered =    IM_COL32(75, 75, 75, 255);
    ImU32 ColorStyle_Outline =              IM_COL32(100, 100, 100, 255);
    ImU32 ColorStyle_OutlineSelected =      IM_COL32(100, 100, 100, 255);
    ImU32 ColorStyle_TitleBar =             IM_COL32(41, 74, 122, 255);
    ImU32 ColorStyle_TitleBarHovered =      IM_COL32(66, 150, 250, 255);

	bool m_opened = true;

public:
	ModuleWindow();

	void drawWindow();
	void setOpenedState(bool state) { m_opened = state; }
	bool getOpenedState() { return m_opened; }

private:
	void drawMenubar();
	void drawModules();
	void drawModule(std::shared_ptr<Module>& m);

};

