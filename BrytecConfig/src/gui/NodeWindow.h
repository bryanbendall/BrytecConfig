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

public:
    enum class Mode {
        Build,
        Simulation
    };

    static inline const float nodeWidth = 140.0f;

private:
    bool m_opened = true;
    bool m_isFocused = false;

    std::weak_ptr<Selectable> m_lastSelected;
    Mode m_mode = Mode::Build;
    std::weak_ptr<NodeGroup> m_nodeGroup;
    imnodes::EditorContext* defaultContext;
    std::map<std::shared_ptr<NodeGroup>, imnodes::EditorContext*> m_contexts;
};
