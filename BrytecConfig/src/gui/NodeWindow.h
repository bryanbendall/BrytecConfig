#pragma once

#include "data/NodeGroup.h"
#include "data/Selectable.h"
#include <imnodes.h>
#include <map>
#include <memory>
#include <utility>
#include <vector>

class NodeWindow {
public:
    enum class Mode {
        Build,
        Simulation
    };

private:
    bool m_opened = true;
    bool m_isFocused = false;

    std::weak_ptr<Selectable> m_lastSelected;
    Mode m_mode = Mode::Build;
    std::weak_ptr<NodeGroup> m_nodeGroup;
    imnodes::EditorContext* defaultContext;
    std::map<std::shared_ptr<NodeGroup>, imnodes::EditorContext*> m_contexts;

public:
    static unsigned int const boolColor = IM_COL32(99, 99, 200, 255); // blue
    static unsigned int const zeroToOneColor = IM_COL32(200, 200, 36, 255); // yellow
    static unsigned int const anyValueColor = IM_COL32(99, 200, 99, 255); // green
    static unsigned int const grayColor = IM_COL32(162, 163, 162, 255); // gray
    static inline const float nodeWidth = 140.0f;

public:
    NodeWindow();
    ~NodeWindow();

    void drawWindow();
    void setOpenedState(bool state) { m_opened = state; }
    bool getOpenedState() { return m_opened; }
    bool getIsWindowFocused() { return m_isFocused; }
    void removeContext(std::shared_ptr<NodeGroup>& nodeGroup);

private:
    void drawMenubar();
    void drawPopupMenu(std::shared_ptr<NodeGroup>& nodeGroup);
    void drawNode(std::shared_ptr<Node>& node);
    void addLinkData(std::shared_ptr<NodeGroup>& nodeGroup);
    void isLinkCreated(std::shared_ptr<NodeGroup>& nodeGroup);
    void isLinkDeleted(std::shared_ptr<NodeGroup>& nodeGroup);
    void isNodeDeleted(std::shared_ptr<NodeGroup>& nodeGroup);
    void saveNodePositions(std::shared_ptr<NodeGroup>& nodeGroup);
    void doValuePopup(std::shared_ptr<NodeGroup>& nodeGroup);
    imnodes::EditorContext* getContext(std::shared_ptr<NodeGroup>& nodeGroup);
};
