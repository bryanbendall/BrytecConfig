#pragma once

#include <string>

class NodeGroupWindow {

public:
    NodeGroupWindow();
    void drawWindow();
    bool getOpenedState() { return m_opened; }
    void setOpenedState(bool state) { m_opened = state; }

private:
    void drawMenubar();
    void drawNodeGroups();

private:
    enum class FilterType {
        All,
        Assigned,
        Unassigned
    };

    bool m_opened = true;
    FilterType m_filter = FilterType::All;
    std::string m_search;
    int m_shownNodeGroups = 0;
};