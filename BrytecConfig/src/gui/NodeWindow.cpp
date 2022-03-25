#include "NodeWindow.h"

#include "AppManager.h"
#include "BrytecConfigEmbedded/Nodes.h"
#include "data/Selectable.h"
#include <IconsFontAwesome5.h>
#include <bitset>
#include <imgui.h>
#include <imgui_internal.h>
#include <iostream>
#include <misc/cpp/imgui_stdlib.h>
#include "NodeUI.h"

NodeWindow::NodeWindow()
{
    imnodes::Initialize();
    imnodes::StyleColorsDark();
    defaultContext = imnodes::EditorContextCreate();
    imnodes::EditorContextSet(defaultContext);

    imnodes::SetNodeGridSpacePos(1, ImVec2(10.0f, 10.0f));

    imnodes::Style& style = imnodes::GetStyle();
    style.colors[imnodes::ColorStyle_NodeBackground] = IM_COL32(50, 50, 50, 230);
    style.colors[imnodes::ColorStyle_TitleBarSelected] = IM_COL32(66, 150, 250, 230);
    style.colors[imnodes::ColorStyle_TitleBarHovered] = IM_COL32(53, 118, 200, 230);
    style.colors[imnodes::ColorStyle_NodeBackgroundSelected] = style.colors[imnodes::ColorStyle_NodeBackground];
    style.colors[imnodes::ColorStyle_NodeBackgroundHovered] = style.colors[imnodes::ColorStyle_NodeBackground];
    style.colors[imnodes::ColorStyle_Pin] = anyValueColor;
    style.colors[imnodes::ColorStyle_PinHovered] = grayColor;
    style.colors[imnodes::ColorStyle_Link] = grayColor;
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

    std::shared_ptr<NodeGroup> nodeGroup = std::dynamic_pointer_cast<NodeGroup>(AppManager::getSelectedItem().lock());
    if (nodeGroup) {
        imnodes::EditorContextSet(getContext(nodeGroup));
        m_nodeGroup = nodeGroup;
    }

    auto pin = std::dynamic_pointer_cast<Pin>(AppManager::getSelectedItem().lock());
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
            if (ImGui::MenuItem(Node::s_nodeName[i], NULL, false)) {
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
        imnodes::PushColorStyle(imnodes::ColorStyle_NodeBackground, IM_COL32(204, 25, 25, 204));
        imnodes::PushColorStyle(imnodes::ColorStyle_NodeBackgroundHovered, IM_COL32(204, 25, 25, 204));
        imnodes::PushColorStyle(imnodes::ColorStyle_NodeBackgroundSelected, IM_COL32(204, 25, 25, 204));
        imnodes::PushColorStyle(imnodes::ColorStyle_TitleBar, IM_COL32(255, 25, 25, 204));
        imnodes::PushColorStyle(imnodes::ColorStyle_TitleBarHovered, IM_COL32(255, 25, 25, 204));
        imnodes::PushColorStyle(imnodes::ColorStyle_TitleBarSelected, IM_COL32(255, 25, 25, 204));
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

    NodeUI::drawNode(node);

    // switch (node->getType()) {
    // case NodeTypes::Initial_Value:
    //     drawInitialValue(node);
    //     break;
    // case NodeTypes::Final_Value:
    //     drawFinalValue(node);
    //     break;
    // case NodeTypes::Node_Group:
    //     drawNodeGroup(node);
    //     break;
    // case NodeTypes::And:
    //     drawAnd(node);
    //     break;
    // case NodeTypes::Or:
    //     drawOr(node);
    //     break;
    // case NodeTypes::Two_Stage:
    //     drawTwoStage(node);
    //     break;
    // case NodeTypes::Curve:
    //     drawCurve(node);
    //     break;
    // case NodeTypes::Compare:
    //     drawCompare(node);
    //     break;
    // case NodeTypes::On_Off:
    //     drawOnOff(node);
    //     break;
    // case NodeTypes::Invert:
    //     drawInvert(node);
    //     break;
    // case NodeTypes::Toggle:
    //     drawToggle(node);
    //     break;
    // case NodeTypes::Delay:
    //     drawDelay(node);
    //     break;
    // case NodeTypes::Push_Button:
    //     drawPushButton(node);
    //     break;
    // case NodeTypes::Map_Value:
    //     drawMapValue(node);
    //     break;
    // case NodeTypes::Math:
    //     drawMath(node);
    //     break;
    // case NodeTypes::Value:
    //     drawValue(node);
    //     break;
    // case NodeTypes::Switch:
    //     drawSwitch(node);
    //     break;
    // case NodeTypes::CanBus:
    //     drawCanBus(node);
    //     break;
    // }

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
                        imnodes::PushColorStyle(imnodes::ColorStyle_Link, IM_COL32(20, 200, 20, 255));
                    } else {
                        //imnodes::PushColorStyle(imnodes::ColorStyle_Link, IM_COL32(200, 20, 20, 255));
                        imnodes::PushColorStyle(imnodes::ColorStyle_Link, grayColor);
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

void NodeWindow::drawFinalValue(std::shared_ptr<Node>& node)
{
    // UI::InputFloat(node, 0, "Final Value");

    // if (m_mode == Mode::Simulation) {

    //     bool onOff = false;
    //     bool floatValue = false;
    //     bool percentValue = false;

    //     if (!m_nodeGroup.expired()) {
    //         auto nodeGroup = m_nodeGroup.lock();
    //         auto type = nodeGroup->getType();
    //         switch (type) {
    //         case IOTypes::Types::Output_12V_Pwm:
    //             percentValue = true;
    //             break;
    //         case IOTypes::Types::Output_12V:
    //         case IOTypes::Types::Output_12V_Low_Current:
    //         case IOTypes::Types::Output_Ground:
    //         case IOTypes::Types::Input_12V:
    //         case IOTypes::Types::Input_Ground:
    //         case IOTypes::Types::Input_5V:
    //             onOff = true;
    //             break;
    //         case IOTypes::Types::Input_5V_Variable:
    //         case IOTypes::Types::Input_Can:
    //             floatValue = true;
    //             break;
    //         }
    //     }

    //     float& value = node->getInputValue(0);

    //     if (onOff) {
    //         UI::OnOffButton(node, value, false);

    //     } else if (floatValue) {
    //         std::stringstream stream;
    //         stream << std::fixed << std::setprecision(2) << value;
    //         UI::SameHeightText(stream.str());
    //     } else if (percentValue) {
    //         ImGui::ProgressBar(value / 100.0f, ImVec2(nodeWidth, 0.0f));
    //     }
    // }
}

void NodeWindow::drawInitialValue(std::shared_ptr<Node>& node)
{
    // if (m_mode == Mode::Simulation) {

    //     UI::ValueFloat(node, 0, "Value", 0.0f, 5.0f, 0.01f);

    //     UI::OnOffButton(node, node->getValue(0), true);
    // }

    // UI::Ouput(node, 0, "Value");
}

void NodeWindow::drawNodeGroup(std::shared_ptr<Node>& node)
{
    // imnodes::BeginStaticAttribute(node->getValueId(0));

    // std::shared_ptr<NodeGroup> thisNodeGroup;
    // if (!m_nodeGroup.expired())
    //     thisNodeGroup = m_nodeGroup.lock();

    // std::shared_ptr<NodeGroup> selectedNodeGroup = AppManager::getConfig()->findNodeGroup(node->getSelectedNodeGroup());

    // if (ImGui::BeginCombo("###pinsCombo", !selectedNodeGroup ? "" : selectedNodeGroup->getName().c_str())) {

    //     for (auto& nodeGroup : AppManager::getConfig()->getNodeGroups()) {

    //         // Skip if it is this node group
    //         if (thisNodeGroup && thisNodeGroup == nodeGroup)
    //             continue;

    //         ImGui::PushID(nodeGroup.get());
    //         bool isSelected = nodeGroup == selectedNodeGroup;
    //         if (ImGui::Selectable(nodeGroup->getName().c_str(), isSelected)) {
    //             node->setSelectedNodeGroup(nodeGroup->getId());
    //         }
    //         ImGui::PopID();

    //         if (isSelected)
    //             ImGui::SetItemDefaultFocus();
    //     }
    //     ImGui::EndCombo();
    // }

    // if (m_mode == Mode::Simulation) {

    //     ImGui::DragFloat("###float1", &node->getValue(0), 1.0f, 0.0f, 10000.0f, "%.2f");

    //     const char* text;
    //     ImU32 color;
    //     ImU32 hoverColor;
    //     if (node->getValue(0) > 0.0f) {
    //         text = "On";
    //         color = IM_COL32(20, 200, 20, 255);
    //         hoverColor = IM_COL32(20, 220, 20, 255);
    //     } else {
    //         text = "Off";
    //         color = IM_COL32(200, 20, 20, 255);
    //         hoverColor = IM_COL32(220, 20, 20, 255);
    //     }
    //     ImGui::PushStyleColor(ImGuiCol_Button, color);
    //     ImGui::PushStyleColor(ImGuiCol_ButtonHovered, hoverColor);
    //     ImGui::PushStyleColor(ImGuiCol_ButtonActive, hoverColor);
    //     ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.0f);
    //     if (ImGui::Button(text, ImVec2(nodeWidth, 0.0f)))
    //         node->getValue(0) > 0.0f ? node->getValue(0) = 0.0f : node->getValue(0) = 1.0f;
    //     ImGui::PopStyleVar();
    //     ImGui::PopStyleColor(3);
    // }

    // imnodes::EndStaticAttribute();

    // UI::Ouput(node, 0, "Result");
}

void NodeWindow::drawAnd(std::shared_ptr<Node>& node)
{
    // UI::InputBool(node, 0, "Input");
    // UI::InputBool(node, 1, "Input");
    // UI::InputBool(node, 2, "Input");
    // UI::InputBool(node, 3, "Input");
    // UI::InputBool(node, 4, "Input");

    // UI::Ouput(node, 0, "Result", boolColor);
}

void NodeWindow::drawOr(std::shared_ptr<Node>& node)
{
    drawAnd(node);
}

void NodeWindow::drawTwoStage(std::shared_ptr<Node>& node)
{
    // UI::InputBool(node, 0, "Stage 1");

    // UI::InputFloat(node, 1, "Stage 1");

    // UI::InputBool(node, 2, "Stage 2");

    // UI::InputFloat(node, 3, "Stage 2");

    // UI::Ouput(node, 0, "Result");
}

void NodeWindow::drawCurve(std::shared_ptr<Node>& node)
{
    // UI::InputBool(node, 0, "Input");

    // UI::ValueCombo(node, 0, Node::s_curveNames, (int)CurveNode::Types::Count);

    // UI::InputBool(node, 1, "Repeat");

    // UI::InputFloat(node, 2, "Time", 0.0f, 10.0f, 0.05f);

    // UI::Ouput(node, 0, "Result", zeroToOneColor);
}

void NodeWindow::drawCompare(std::shared_ptr<Node>& node)
{
    // UI::InputFloat(node, 0, "Value 1");

    // UI::InputFloat(node, 1, "Value 2");

    // UI::ValueCombo(node, 0, Node::s_compareNames, (int)CompareNode::Types::Count);

    // UI::Ouput(node, 0, "Result", boolColor);
}

void NodeWindow::drawOnOff(std::shared_ptr<Node>& node)
{
    // UI::InputBool(node, 0, "On");

    // UI::InputBool(node, 1, "Off");

    // UI::Ouput(node, 0, "Output", boolColor);
}

void NodeWindow::drawInvert(std::shared_ptr<Node>& node)
{
    // UI::InputBool(node, 0, "Input");

    // UI::Ouput(node, 0, "Output", boolColor);
}

void NodeWindow::drawToggle(std::shared_ptr<Node>& node)
{
    // UI::InputBool(node, 0, "Input");

    // UI::Ouput(node, 0, "Output", boolColor);
}

void NodeWindow::drawDelay(std::shared_ptr<Node>& node)
{
    // UI::InputFloat(node, 0, "Input");

    // UI::InputFloat(node, 1, "Time");

    // UI::Ouput(node, 0, "Output");
}

void NodeWindow::drawPushButton(std::shared_ptr<Node>& node)
{
    // UI::InputBool(node, 0, "Button");

    // UI::InputBool(node, 1, "Neutral Safety");

    // UI::InputBool(node, 2, "Enging Running");

    // UI::Ouput(node, 0, "Ignition", boolColor);

    // UI::Ouput(node, 1, "Starter", boolColor);
}

void NodeWindow::drawMapValue(std::shared_ptr<Node>& node)
{
    // UI::InputFloat(node, 0, "Input");

    // UI::InputFloat(node, 1, "From Min");
    // UI::InputFloat(node, 2, "From Max");

    // UI::InputFloat(node, 3, "To Min");
    // UI::InputFloat(node, 4, "To Max");

    // UI::Ouput(node, 0, "Result");
}

void NodeWindow::drawMath(std::shared_ptr<Node>& node)
{
    // UI::InputFloat(node, 0, "Value");

    // UI::InputFloat(node, 1, "Value");

    // UI::ValueCombo(node, 0, Node::s_mathNames, (int)MathNode::Types::Count);

    // UI::Ouput(node, 0, "Result");
}

void NodeWindow::drawValue(std::shared_ptr<Node>& node)
{
    // UI::ValueFloat(node, 0, "Value");

    // UI::Ouput(node, 0, "Output");
}

void NodeWindow::drawSwitch(std::shared_ptr<Node>& node)
{
    // UI::InputBool(node, 0, "Selection");

    // UI::InputFloat(node, 1, "If True");

    // UI::InputFloat(node, 2, "If False");

    // UI::Ouput(node, 0, "Output");
}

void NodeWindow::drawCanBus(std::shared_ptr<Node>& node)
{
    // UI::Ouput(node, 0, "Id");
    // UI::Ouput(node, 1, "Data 0");
    // UI::Ouput(node, 2, "Data 1");
    // UI::Ouput(node, 3, "Data 2");
    // UI::Ouput(node, 4, "Data 3");
    // UI::Ouput(node, 5, "Data 4");
    // UI::Ouput(node, 6, "Data 5");
    // UI::Ouput(node, 7, "Data 6");
    // UI::Ouput(node, 8, "Data 7");
}
