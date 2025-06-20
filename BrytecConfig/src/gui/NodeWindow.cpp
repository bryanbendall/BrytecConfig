#include "NodeWindow.h"

#include "AppManager.h"
#include "NodeUI.h"
#include "data/Pin.h"
#include "data/Selectable.h"
#include "utils/Colors.h"
#include <IconsFontAwesome5.h>
#include <bitset>
#include <imgui.h>
#include <imgui_internal.h>
#include <iostream>
#include <misc/cpp/imgui_stdlib.h>

namespace Brytec {

NodeWindow::NodeWindow()
{
    ImNodes::CreateContext();
    ImNodes::StyleColorsDark();
    defaultContext = ImNodes::EditorContextCreate();
    ImNodes::EditorContextSet(defaultContext);

    ImNodes::SetNodeGridSpacePos(1, ImVec2(10.0f, 10.0f));

    ImNodesStyle& style = ImNodes::GetStyle();
    style.Colors[ImNodesCol_NodeBackground] = Colors::Node::NodeBackground;
    style.Colors[ImNodesCol_TitleBarSelected] = Colors::Node::TitleBarSelected;
    style.Colors[ImNodesCol_TitleBarHovered] = Colors::Node::TitleBarHovered;
    style.Colors[ImNodesCol_NodeBackgroundSelected] = style.Colors[ImNodesCol_NodeBackground];
    style.Colors[ImNodesCol_NodeBackgroundHovered] = style.Colors[ImNodesCol_NodeBackground];
    style.Colors[ImNodesCol_Pin] = Colors::NodeConnections::AnyValue;
    style.Colors[ImNodesCol_PinHovered] = Colors::NodeConnections::Gray;
    style.Colors[ImNodesCol_Link] = Colors::NodeConnections::Gray;
}

NodeWindow::~NodeWindow()
{
    ImNodes::EditorContextFree(defaultContext);

    for (auto c : m_contexts)
        ImNodes::EditorContextFree(c.second);
}

void NodeWindow::drawWindow()
{
    if (!m_opened)
        return;

    // Reset context in case it has been deleted
    ImNodes::EditorContextSet(defaultContext);

    m_nodeGroup.reset();

    std::shared_ptr<NodeGroup> nodeGroup = AppManager::getSelected<NodeGroup>();
    if (nodeGroup) {
        ImNodes::EditorContextSet(getContext(nodeGroup));
        m_nodeGroup = nodeGroup;
    }

    auto pin = AppManager::getSelected<Pin>();
    if (pin && pin->getNodeGroup()) {
        nodeGroup = pin->getNodeGroup();
        ImNodes::EditorContextSet(getContext(nodeGroup));
        m_nodeGroup = nodeGroup;
    }

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin(ICON_FA_PROJECT_DIAGRAM " Node Editor", &m_opened, ImGuiWindowFlags_MenuBar);
    ImGui::PopStyleVar();

    bool windowFocus = ImGui::IsWindowFocused(ImGuiFocusedFlags_DockHierarchy);

    drawMenubar();

    ImNodes::BeginNodeEditor();

    bool nodeGraphFocus = ImGui::IsWindowFocused(ImGuiFocusedFlags_DockHierarchy);
    m_isFocused = windowFocus || nodeGraphFocus;

    float linkThickness = std::clamp(AppManager::getZoom() / 5.0f, 2.0f, 6.0f);
    ImNodes::PushStyleVar(ImNodesStyleVar_LinkThickness, linkThickness);
    ImNodes::PushStyleVar(ImNodesStyleVar_PinCircleRadius, linkThickness);
    ImNodes::PushStyleVar(ImNodesStyleVar_NodePadding, ImVec2(AppManager::getZoom() / 2.0f, 4.0f));

    drawPopupMenu(nodeGroup);

    if (nodeGroup) {
        if (nodeGroup->getNodes().size() > 0) {
            for (auto& n : nodeGroup->getNodes())
                drawNode(n);
        }

        addLinkData(nodeGroup);
    }

    ImNodes::MiniMap(0.2f, ImNodesMiniMapLocation_BottomRight);
    ImNodes::EndNodeEditor();

    ImNodes::PopStyleVar();
    ImNodes::PopStyleVar();
    ImNodes::PopStyleVar();

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
    ImNodes::EditorContextFree(m_contexts[nodeGroup]);
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
        if (ImNodes::IsEditorHovered()) {
            open_context_menu = true;
        }
    if (open_context_menu) {
        ImGui::OpenPopup("NodeWindowPopup");
    }

    // Draw context menu
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
    if (ImGui::BeginPopup("NodeWindowPopup")) {

        m_popupPosition = ImGui::GetMousePosOnOpeningCurrentPopup();

        if (ImGui::BeginMenu("Node Group")) {
            drawPopupMenuItem(nodeGroup, NodeTypes::Initial_Value);
            drawPopupMenuItem(nodeGroup, NodeTypes::Node_Group);
            drawPopupMenuItem(nodeGroup, NodeTypes::PinCurrent);
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Control")) {
            drawPopupMenuItem(nodeGroup, NodeTypes::Switch);
            drawPopupMenuItem(nodeGroup, NodeTypes::On_Off);
            drawPopupMenuItem(nodeGroup, NodeTypes::Toggle);
            drawPopupMenuItem(nodeGroup, NodeTypes::Delay);
            drawPopupMenuItem(nodeGroup, NodeTypes::PID);
            drawPopupMenuItem(nodeGroup, NodeTypes::Counter);
            drawPopupMenuItem(nodeGroup, NodeTypes::Two_Stage);
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Boolean")) {
            drawPopupMenuItem(nodeGroup, NodeTypes::And);
            drawPopupMenuItem(nodeGroup, NodeTypes::Or);
            drawPopupMenuItem(nodeGroup, NodeTypes::Compare);
            drawPopupMenuItem(nodeGroup, NodeTypes::Invert);
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Math")) {
            drawPopupMenuItem(nodeGroup, NodeTypes::Value);
            drawPopupMenuItem(nodeGroup, NodeTypes::Math);
            drawPopupMenuItem(nodeGroup, NodeTypes::Map_Value);
            drawPopupMenuItem(nodeGroup, NodeTypes::Curve);
            drawPopupMenuItem(nodeGroup, NodeTypes::Interpolate);
            drawPopupMenuItem(nodeGroup, NodeTypes::Clamp);
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Can Bus")) {
            drawPopupMenuItem(nodeGroup, NodeTypes::CanBusInput);
            drawPopupMenuItem(nodeGroup, NodeTypes::CanBusOutput);
            drawPopupMenuItem(nodeGroup, NodeTypes::Holley_Broadcast);
            drawPopupMenuItem(nodeGroup, NodeTypes::Holley_Io_Module);
            drawPopupMenuItem(nodeGroup, NodeTypes::Racepak_Switch_Panel);
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Color")) {
            drawPopupMenuItem(nodeGroup, NodeTypes::Color);
            drawPopupMenuItem(nodeGroup, NodeTypes::Mix_Color);
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Special")) {
            drawPopupMenuItem(nodeGroup, NodeTypes::Thermistor);
            ImGui::EndMenu();
        }

        ImGui::EndPopup();
    }
    ImGui::PopStyleVar();
}

void NodeWindow::drawPopupMenuItem(std::shared_ptr<NodeGroup>& nodeGroup, NodeTypes type)
{
    // Can only have one of these nodes
    bool enabled = !(type == NodeTypes::PinCurrent && nodeGroup->hasNodeType(NodeTypes::PinCurrent));
    enabled &= !(type == NodeTypes::Initial_Value && nodeGroup->hasNodeType(NodeTypes::Initial_Value));

    if (ImGui::MenuItem(Node::s_nodeName[(int)type], NULL, false, enabled)) {
        nodeGroup->addNode(type);
        ImNodes::SetNodeScreenSpacePos(nodeGroup->getNodes().back()->getId(), m_popupPosition);
    }
}

void NodeWindow::drawNode(std::shared_ptr<Node>& node)
{

    if (!node)
        return;

    if (m_lastSelected.lock() != AppManager::getSelectedItem().lock())
        ImNodes::SetNodeGridSpacePos(node->getId(), node->getPosition());

    // Style for nodes
    ImVec2 label_size = ImGui::CalcTextSize("This is a node box size");
    if (node->getLoopFound()) {
        ImNodes::PushColorStyle(ImNodesCol_NodeBackground, Colors::Node::Error);
        ImNodes::PushColorStyle(ImNodesCol_NodeBackgroundHovered, Colors::Node::Error);
        ImNodes::PushColorStyle(ImNodesCol_NodeBackgroundSelected, Colors::Node::Error);
        ImNodes::PushColorStyle(ImNodesCol_TitleBar, Colors::Node::Error);
        ImNodes::PushColorStyle(ImNodesCol_TitleBarHovered, Colors::Node::Error);
        ImNodes::PushColorStyle(ImNodesCol_TitleBarSelected, Colors::Node::Error);
    }

    ImNodes::BeginNode(node->getId());

    // Node title
    ImNodes::BeginNodeTitleBar();
    ImGui::PushItemWidth(label_size.x);

    static int editingId = -1;
    static bool setFocus = false;
    ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(0, 0, 0, 0));
    if (ImGui::Button(ICON_FA_EDIT)) {
        editingId = node->getId();
        setFocus = true;
    }
    ImGui::PopStyleColor();
    ImGui::SameLine();

    ImGui::PushItemWidth(label_size.x - 30);
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

    ImNodes::EndNodeTitleBar();

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10.0f, 3.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(2.0f, 4.0f));

    NodeUI::drawNode(node, m_mode, m_nodeGroup, label_size.x);

    ImGui::PopStyleVar(2);
    ImGui::PopItemWidth();
    ImNodes::EndNode();

    if (node->getLoopFound()) {
        ImNodes::PopColorStyle();
        ImNodes::PopColorStyle();
        ImNodes::PopColorStyle();
        ImNodes::PopColorStyle();
        ImNodes::PopColorStyle();
        ImNodes::PopColorStyle();
    }
}

void NodeWindow::addLinkData(std::shared_ptr<NodeGroup>& nodeGroup)
{
    if (!nodeGroup)
        return;

    for (int i = 0; i < nodeGroup->getNodes().size(); i++) {
        auto& fromNode = nodeGroup->getNodes()[i];
        if (!fromNode)
            continue;
        for (int fromLinkAttribute = 0; fromLinkAttribute < fromNode->getInputs().size(); fromLinkAttribute++) {
            auto& toNode = fromNode->getInput(fromLinkAttribute).ConnectedNode;

            if (!toNode.expired()) {

                if (m_mode == Mode::Simulation) {
                    float outputValue = fromNode->getInput(fromLinkAttribute).ConnectedNode.lock()->getOutput(fromNode->getInput(fromLinkAttribute).OutputIndex);
                    if (outputValue > 0.0f) {
                        ImNodes::PushColorStyle(ImNodesCol_Link, Colors::NodeConnections::LinkOn);
                    } else {
                        ImNodes::PushColorStyle(ImNodesCol_Link, Colors::NodeConnections::Gray);
                    }
                }

                ImNodes::Link(
                    (fromNode->getId() << 16) + (toNode.lock()->getId() << 8) + fromLinkAttribute,
                    (toNode.lock()->getId() << 8) + fromNode->getInput(fromLinkAttribute).OutputIndex,
                    (fromNode->getId() << 8) + fromLinkAttribute + fromNode->getOutputs().size());

                if (m_mode == Mode::Simulation) {
                    ImNodes::PopColorStyle();
                }
            }
        }
    }
}

void NodeWindow::isLinkCreated(std::shared_ptr<NodeGroup>& nodeGroup)
{
    int begin, end;
    if (ImNodes::IsLinkCreated(&begin, &end)) {
        unsigned char beginNodeIndex = begin >> 8;
        unsigned char beginAttributeIndex = begin & 0xFF;
        unsigned char endNodeIndex = end >> 8;
        unsigned char endAttributeIndex = end & 0xFF;

        // Cant link to itself
        if (beginNodeIndex == endNodeIndex)
            return;

        if (!nodeGroup)
            return;

        // Check if start node is an output
        if (beginAttributeIndex < nodeGroup->getNode(beginNodeIndex)->getOutputs().size()) {
            // If also an output return
            if (endAttributeIndex < nodeGroup->getNode(endNodeIndex)->getOutputs().size())
                return;
            auto& nodeConnection = nodeGroup->getNode(endNodeIndex)->getInput(endAttributeIndex - nodeGroup->getNode(endNodeIndex)->getOutputs().size());
            nodeConnection.ConnectedNode = nodeGroup->getNode(beginNodeIndex);
            nodeConnection.OutputIndex = beginAttributeIndex;
            nodeGroup->sortNodes();
            // std::cout << "saving output to input\n";
            return;
        }

        // Then start node is an input
        // Make sure the end node is an output
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
    const int num_selected = ImNodes::NumSelectedLinks();
    if (num_selected > 0 && ImGui::IsKeyReleased(ImGui::GetKeyIndex(ImGuiKey_Delete))) {
        static std::vector<int> selected_links;
        selected_links.resize(static_cast<size_t>(num_selected));
        ImNodes::GetSelectedLinks(selected_links.data());
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
    const int num_selected = ImNodes::NumSelectedNodes();
    if (num_selected > 0 && ImGui::IsKeyReleased(ImGui::GetKeyIndex(ImGuiKey_Delete))) {
        static std::vector<int> selected_nodes;
        selected_nodes.resize(static_cast<size_t>(num_selected));
        ImNodes::GetSelectedNodes(selected_nodes.data());
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
        n->getPosition() = ImNodes::GetNodeGridSpacePos(n->getId());
    }
}

void NodeWindow::doValuePopup(std::shared_ptr<NodeGroup>& nodeGroup)
{
    int hovered;
    if (m_mode == Mode::Simulation && ImNodes::IsPinHovered(&hovered))
        ImGui::SetTooltip("%.2f", nodeGroup->getValue(hovered));
}

ImNodesEditorContext* NodeWindow::getContext(std::shared_ptr<NodeGroup>& nodeGroup)
{

    if (m_contexts.find(nodeGroup) == m_contexts.end())
        m_contexts[nodeGroup] = ImNodes::EditorContextCreate();

    return m_contexts[nodeGroup];
}

}