#include "NodeWindow.h"

#include "AppManager.h"
#include "NodeUI.h"
#include "data/Selectable.h"
#include "utils/Colors.h"
#include <IconsFontAwesome5.h>
#include <bitset>
#include <imgui.h>
#include <imgui_internal.h>
#include <iostream>
#include <misc/cpp/imgui_stdlib.h>

#include "BrytecConfigEmbedded/Deserializer/BinaryDeserializer.h"
#include "BrytecConfigEmbedded/Nodes/EMathNode.h"
#include "utils/BinarySerializer.h"

NodeWindow::NodeWindow()
{
    imnodes::Initialize();
    imnodes::StyleColorsDark();
    defaultContext = imnodes::EditorContextCreate();
    imnodes::EditorContextSet(defaultContext);

    imnodes::SetNodeGridSpacePos(1, ImVec2(10.0f, 10.0f));

    imnodes::Style& style = imnodes::GetStyle();
    style.colors[imnodes::ColorStyle_NodeBackground] = Colors::Node::NodeBackground;
    style.colors[imnodes::ColorStyle_TitleBarSelected] = Colors::Node::TitleBarSelected;
    style.colors[imnodes::ColorStyle_TitleBarHovered] = Colors::Node::TitleBarHovered;
    style.colors[imnodes::ColorStyle_NodeBackgroundSelected] = style.colors[imnodes::ColorStyle_NodeBackground];
    style.colors[imnodes::ColorStyle_NodeBackgroundHovered] = style.colors[imnodes::ColorStyle_NodeBackground];
    style.colors[imnodes::ColorStyle_Pin] = Colors::NodeConnections::AnyValue;
    style.colors[imnodes::ColorStyle_PinHovered] = Colors::NodeConnections::Gray;
    style.colors[imnodes::ColorStyle_Link] = Colors::NodeConnections::Gray;
}

NodeWindow::~NodeWindow()
{
    imnodes::EditorContextFree(defaultContext);

    for (auto c : m_contexts)
        imnodes::EditorContextFree(c.second);
}

void NodeWindow::drawWindow()
{
    if (!m_opened)
        return;

    // Reset context in case it has been deleted
    imnodes::EditorContextSet(defaultContext);

    m_nodeGroup.reset();

    std::shared_ptr<NodeGroup> nodeGroup = AppManager::getSelected<NodeGroup>();
    if (nodeGroup) {
        imnodes::EditorContextSet(getContext(nodeGroup));
        m_nodeGroup = nodeGroup;
    }

    auto pin = AppManager::getSelected<Pin>();
    if (pin && pin->getNodeGroup()) {
        nodeGroup = pin->getNodeGroup();
        imnodes::EditorContextSet(getContext(nodeGroup));
        m_nodeGroup = nodeGroup;
    }

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin(ICON_FA_PROJECT_DIAGRAM " Node Editor", &m_opened, ImGuiWindowFlags_MenuBar);
    ImGui::PopStyleVar();

    bool windowFocus = ImGui::IsWindowFocused(ImGuiFocusedFlags_DockHierarchy);

    drawMenubar();

    imnodes::BeginNodeEditor();

    bool nodeGraphFocus = ImGui::IsWindowFocused(ImGuiFocusedFlags_DockHierarchy);
    m_isFocused = windowFocus || nodeGraphFocus;

    imnodes::PushStyleVar(imnodes::StyleVar_NodePaddingVertical, 4.0f);

    drawPopupMenu(nodeGroup);

    if (nodeGroup) {
        if (nodeGroup->getNodes().size() > 0) {
            for (auto& n : nodeGroup->getNodes())
                drawNode(n);
        }

        addLinkData(nodeGroup);
    }

    imnodes::EndNodeEditor();

    imnodes::PopStyleVar();

    if (nodeGroup) {
        saveNodePositions(nodeGroup);

        isLinkCreated(nodeGroup);
        isLinkDeleted(nodeGroup);
        isNodeDeleted(nodeGroup);

        doValuePopup(nodeGroup);
    }
    ImGui::End();

    if (nodeGroup && m_mode == Mode::Simulation)
        nodeGroup->evaluateAllNodes();

    m_lastSelected = AppManager::getSelectedItem();
}

void NodeWindow::removeContext(std::shared_ptr<NodeGroup>& nodeGroup)
{
    imnodes::EditorContextFree(m_contexts[nodeGroup]);
    m_contexts.erase(nodeGroup);
}

void NodeWindow::drawMenubar()
{
    if (ImGui::BeginMenuBar()) {

        bool buildMode = (m_mode == Mode::Build);
        if (ImGui::Selectable("Building Mode", buildMode, 0, ImVec2(ImGui::CalcTextSize("Building Mode").x, 0))) {
            m_mode = Mode::Build;
        }
        ImGui::SameLine();
        if (ImGui::Selectable("Simulation Mode", !buildMode, 0, ImVec2(ImGui::CalcTextSize("Simulation Mode").x, 0))) {
            m_mode = Mode::Simulation;
        }

        ImGui::EndMenuBar();
    }
}

void NodeWindow::drawPopupMenu(std::shared_ptr<NodeGroup>& nodeGroup)
{

    if (!nodeGroup)
        return;

    bool open_context_menu = false;

    // Open context menu
    if (ImGui::IsMouseReleased(ImGuiMouseButton_Right))
        if (imnodes::IsEditorHovered()) {
            open_context_menu = true;
        }
    if (open_context_menu) {
        ImGui::OpenPopup("NodeWindowPopup");
    }

    // Draw context menu
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
    if (ImGui::BeginPopup("NodeWindowPopup")) {
        ImVec2 scene_pos = ImGui::GetMousePosOnOpeningCurrentPopup();

        for (int i = (int)NodeTypes::Initial_Value; i < (int)NodeTypes::Count; i++) {

            // Can only have one of these nodes
            bool enabled = !(i == (int)NodeTypes::PinCurrent && nodeGroup->hasNodeType(NodeTypes::PinCurrent));
            enabled &= !(i == (int)NodeTypes::Initial_Value && nodeGroup->hasNodeType(NodeTypes::Initial_Value));

            if (ImGui::MenuItem(Node::s_nodeName[i], NULL, false, enabled)) {
                nodeGroup->addNode((NodeTypes)i);
                imnodes::SetNodeScreenSpacePos(nodeGroup->getNodes().back()->getId(), scene_pos);
            }
        }

        ImGui::EndPopup();
    }
    ImGui::PopStyleVar();
}

void NodeWindow::drawNode(std::shared_ptr<Node>& node)
{

    if (!node)
        return;

    if (m_lastSelected.lock() != AppManager::getSelectedItem().lock())
        imnodes::SetNodeGridSpacePos(node->getId(), node->getPosition());

    // Style for nodes
    if (node->getLoopFound()) {
        imnodes::PushColorStyle(imnodes::ColorStyle_NodeBackground, Colors::Node::Error);
        imnodes::PushColorStyle(imnodes::ColorStyle_NodeBackgroundHovered, Colors::Node::Error);
        imnodes::PushColorStyle(imnodes::ColorStyle_NodeBackgroundSelected, Colors::Node::Error);
        imnodes::PushColorStyle(imnodes::ColorStyle_TitleBar, Colors::Node::Error);
        imnodes::PushColorStyle(imnodes::ColorStyle_TitleBarHovered, Colors::Node::Error);
        imnodes::PushColorStyle(imnodes::ColorStyle_TitleBarSelected, Colors::Node::Error);
    }

    imnodes::BeginNode(node->getId());

    // Node title
    imnodes::BeginNodeTitleBar();
    ImGui::PushItemWidth(nodeWidth);

    static int editingId = -1;
    static bool setFocus = false;
    ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(0, 0, 0, 0));
    if (ImGui::Button(ICON_FA_EDIT)) {
        editingId = node->getId();
        setFocus = true;
    }
    ImGui::PopStyleColor();
    ImGui::SameLine();

    ImGui::PushItemWidth(nodeWidth - 30);
    if (editingId == node->getId()) {

        // Set focus
        if (setFocus) {
            ImGui::SetKeyboardFocusHere();
            setFocus = false;
        }

        ImGui::InputText("###String", &node->getName(), ImGuiInputTextFlags_AutoSelectAll);

        // End editing
        if (ImGui::IsItemDeactivated())
            editingId = -1;

    } else {
        if (node->getName().empty())
            ImGui::LabelText("###Label", "%s", node->s_getTypeName(node->getType()));
        else
            ImGui::LabelText("###Label", "%s", node->getName().c_str());
    }
    ImGui::PopItemWidth();

    imnodes::EndNodeTitleBar();

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10.0f, 3.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(2.0f, 4.0f));

    NodeUI::drawNode(node, m_mode, m_nodeGroup);

    ImGui::PopStyleVar(2);
    ImGui::PopItemWidth();
    imnodes::EndNode();

    if (node->getLoopFound()) {
        imnodes::PopColorStyle();
        imnodes::PopColorStyle();
        imnodes::PopColorStyle();
        imnodes::PopColorStyle();
        imnodes::PopColorStyle();
        imnodes::PopColorStyle();
    }
}

void NodeWindow::addLinkData(std::shared_ptr<NodeGroup>& nodeGroup)
{
    if (!nodeGroup)
        return;

    for (size_t i = 0; i < nodeGroup->getNodes().size(); i++) {
        auto& fromNode = nodeGroup->getNodes()[i];
        if (!fromNode)
            continue;
        for (size_t fromLinkAttribute = 0; fromLinkAttribute < fromNode->getInputs().size(); fromLinkAttribute++) {
            auto& toNode = fromNode->getInput(fromLinkAttribute).ConnectedNode;

            if (!toNode.expired()) {

                if (m_mode == Mode::Simulation) {
                    float outputValue = fromNode->getInput(fromLinkAttribute).ConnectedNode.lock()->getOutput(fromNode->getInput(fromLinkAttribute).OutputIndex);
                    if (outputValue > 0.0f) {
                        imnodes::PushColorStyle(imnodes::ColorStyle_Link, Colors::NodeConnections::LinkOn);
                    } else {
                        imnodes::PushColorStyle(imnodes::ColorStyle_Link, Colors::NodeConnections::Gray);
                    }
                }

                imnodes::Link(
                    (fromNode->getId() << 16) + (toNode.lock()->getId() << 8) + fromLinkAttribute,
                    (toNode.lock()->getId() << 8) + fromNode->getInput(fromLinkAttribute).OutputIndex,
                    (fromNode->getId() << 8) + fromLinkAttribute + fromNode->getOutputs().size());

                if (m_mode == Mode::Simulation) {
                    imnodes::PopColorStyle();
                }
            }
        }
    }
}

void NodeWindow::isLinkCreated(std::shared_ptr<NodeGroup>& nodeGroup)
{
    int begin, end;
    if (imnodes::IsLinkCreated(&begin, &end)) {
        unsigned char beginNodeIndex = begin >> 8;
        unsigned char beginAttributeIndex = begin & 0xFF;
        unsigned char endNodeIndex = end >> 8;
        unsigned char endAttributeIndex = end & 0xFF;

        //Cant link to itself
        if (beginNodeIndex == endNodeIndex)
            return;

        if (!nodeGroup)
            return;

        //Check if start node is an output
        if (beginAttributeIndex < nodeGroup->getNode(beginNodeIndex)->getOutputs().size()) {
            //If also an output return
            if (endAttributeIndex < nodeGroup->getNode(endNodeIndex)->getOutputs().size())
                return;
            auto& nodeConnection = nodeGroup->getNode(endNodeIndex)->getInput(endAttributeIndex - nodeGroup->getNode(endNodeIndex)->getOutputs().size());
            nodeConnection.ConnectedNode = nodeGroup->getNode(beginNodeIndex);
            nodeConnection.OutputIndex = beginAttributeIndex;
            nodeGroup->sortNodes();
            //std::cout << "saving output to input\n";
            return;
        }

        //Then start node is an input
        //Make sure the end node is an output
        if (endAttributeIndex >= nodeGroup->getNode(endNodeIndex)->getOutputs().size())
            return;
        auto& nodeConnection = nodeGroup->getNode(beginNodeIndex)->getInput(beginAttributeIndex);
        nodeConnection.ConnectedNode = nodeGroup->getNode(endNodeIndex);
        nodeConnection.OutputIndex = endAttributeIndex;
        nodeGroup->sortNodes();
    }
}

void NodeWindow::isLinkDeleted(std::shared_ptr<NodeGroup>& nodeGroup)
{
    const int num_selected = imnodes::NumSelectedLinks();
    if (num_selected > 0 && ImGui::IsKeyReleased(ImGui::GetKeyIndex(ImGuiKey_Delete))) {
        static std::vector<int> selected_links;
        selected_links.resize(static_cast<size_t>(num_selected));
        imnodes::GetSelectedLinks(selected_links.data());
        for (const int link_id : selected_links) {
            int nodeId = (link_id >> 16) & 0xFF;
            int linkAttribute = link_id & 0xFF;

            nodeGroup->getNode(nodeId)->getInput(linkAttribute).ConnectedNode.reset();
        }

        if (nodeGroup)
            nodeGroup->sortNodes();
    }
}

void NodeWindow::isNodeDeleted(std::shared_ptr<NodeGroup>& nodeGroup)
{
    const int num_selected = imnodes::NumSelectedNodes();
    if (num_selected > 0 && ImGui::IsKeyReleased(ImGui::GetKeyIndex(ImGuiKey_Delete))) {
        static std::vector<int> selected_nodes;
        selected_nodes.resize(static_cast<size_t>(num_selected));
        imnodes::GetSelectedNodes(selected_nodes.data());
        for (const int node_id : selected_nodes) {

            if (!nodeGroup)
                return;

            nodeGroup->deleteNode(node_id);
            nodeGroup->sortNodes();
        }
    }
}

void NodeWindow::saveNodePositions(std::shared_ptr<NodeGroup>& nodeGroup)
{
    if (!nodeGroup)
        return;

    for (auto n : nodeGroup->getNodes()) {
        n->getPosition() = imnodes::GetNodeGridSpacePos(n->getId());
    }
}

void NodeWindow::doValuePopup(std::shared_ptr<NodeGroup>& nodeGroup)
{
    int hovered;
    if (m_mode == Mode::Simulation && imnodes::IsPinHovered(&hovered))
        ImGui::SetTooltip("%.2f", nodeGroup->getValue(hovered));
}

imnodes::EditorContext* NodeWindow::getContext(std::shared_ptr<NodeGroup>& nodeGroup)
{

    if (m_contexts.find(nodeGroup) == m_contexts.end())
        m_contexts[nodeGroup] = imnodes::EditorContextCreate();

    return m_contexts[nodeGroup];
}
