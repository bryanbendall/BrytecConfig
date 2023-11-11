#pragma once

#define IMGUI_DEFINE_MATH_OPERATORS

#include "data/NodeGroup.h"
#include "data/Selectable.h"
#include <imnodes.h>
#include <map>
#include <memory>
#include <utility>
#include <vector>

namespace Brytec {

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
    ImNodesEditorContext* getContext(std::shared_ptr<NodeGroup>& nodeGroup);

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
    ImNodesEditorContext* defaultContext;
    std::map<std::shared_ptr<NodeGroup>, ImNodesEditorContext*> m_contexts;
};

}