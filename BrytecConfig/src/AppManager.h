#pragma once

#include "data/Config.h"
#include "data/Selectable.h"
#include <memory>

struct AppManagerData {
	Config m_config;
	std::weak_ptr<Selectable> m_selectedItem;
};

class AppManager
{

	static AppManagerData s_data;

	AppManager() = default;

public:
	static void update();
	static Config& getConfig() { return s_data.m_config; }
	static std::weak_ptr<Selectable> getSelectedItem() { return s_data.m_selectedItem; }
	static void setSelected(std::weak_ptr<Selectable> sel) { s_data.m_selectedItem = sel; }

};

