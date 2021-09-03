#pragma once

class ModuleBuilderWindow
{
	bool m_showWindow = false;

public:
	ModuleBuilderWindow();
	void drawWindow();
	void showWindow() { m_showWindow = true; }

};