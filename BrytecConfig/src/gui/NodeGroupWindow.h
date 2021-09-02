#pragma once

class NodeGroupWindow {

	bool m_opened = true;

public:
	NodeGroupWindow();
	void drawWindow();
	void drawNodeGroups();
	void setOpenedState(bool state) { m_opened = state; }

};