#pragma once

#include "../data/Module.h"
#include "../data/Pin.h"

class PropertiesWindow
{
	bool m_opened = true;

public:
	PropertiesWindow() = default;

	void drawWindow();
	void setOpenedState(bool state) { m_opened = state; }
	bool getOpenedState() { return m_opened; }

private:
	void drawModuleProps(std::shared_ptr<Module> module);
	void drawPinProps(std::shared_ptr<Pin> pin);
	void drawNodeGroupProps(std::shared_ptr<NodeGroup> nodeGroup);

};

