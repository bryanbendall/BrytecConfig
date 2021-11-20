#include "NodeWindow.h"

#include <imgui.h>
#include <imgui_internal.h>
#include "../data/Selectable.h"
#include "../AppManager.h"
#include <iostream>
#include <bitset>
#include <IconsFontAwesome5.h>
#include "../embedded/Nodes.h"

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

    for(auto c : m_contexts)
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
    if(nodeGroup) {
        imnodes::EditorContextSet(getContext(nodeGroup));
        m_nodeGroup = nodeGroup;
    }

    auto pin = std::dynamic_pointer_cast<Pin>(AppManager::getSelectedItem().lock());
    if(pin && pin->getNodeGroup()) {
        nodeGroup = pin->getNodeGroup();
        imnodes::EditorContextSet(getContext(nodeGroup));
        m_nodeGroup = nodeGroup;
    }

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin(ICON_FA_PROJECT_DIAGRAM" Node Editor", &m_opened, ImGuiWindowFlags_MenuBar);
    ImGui::PopStyleVar();

    bool windowFocus = ImGui::IsWindowFocused(ImGuiFocusedFlags_DockHierarchy);

    drawMenubar();

    imnodes::BeginNodeEditor();

    bool nodeGraphFocus = ImGui::IsWindowFocused(ImGuiFocusedFlags_DockHierarchy);
    m_isFocused = windowFocus || nodeGraphFocus;

    imnodes::PushStyleVar(imnodes::StyleVar_NodePaddingVertical, 5.0f);

    drawPopupMenu(nodeGroup);
    
    if(nodeGroup) {
        if(nodeGroup->getNodes().size() > 0) {
            for(auto& n : nodeGroup->getNodes())
                drawNode(n);
        }

        addLinkData(nodeGroup);
    }

    imnodes::EndNodeEditor();
    
    if(nodeGroup) {
        saveNodePositions(nodeGroup);

        isLinkCreated(nodeGroup);
        isLinkDeleted(nodeGroup);
        isNodeDeleted(nodeGroup);

        doValuePopup(nodeGroup);
    }
    ImGui::End();

    if(nodeGroup && m_mode == Mode::Simulation)
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
    if(ImGui::BeginMenuBar()) {

        bool buildMode = (m_mode == Mode::Build);
        if(ImGui::Selectable("Building Mode", buildMode, 0, ImVec2(ImGui::CalcTextSize("Building Mode").x, 0))) { m_mode = Mode::Build; }
        ImGui::SameLine();
        if(ImGui::Selectable("Simulation Mode", !buildMode, 0, ImVec2(ImGui::CalcTextSize("Simulation Mode").x, 0))) { m_mode = Mode::Simulation; }

        ImGui::EndMenuBar();
    }
}

void NodeWindow::drawPopupMenu(std::shared_ptr<NodeGroup>& nodeGroup) 
{

    if(!nodeGroup)
        return;

    bool open_context_menu = false;

    // Open context menu
    if (ImGui::IsMouseReleased(ImGuiMouseButton_Right))
        if (imnodes::IsEditorHovered())
        {
            open_context_menu = true; 
        }
    if (open_context_menu)
    {
        ImGui::OpenPopup("NodeWindowPopup");
    }

    // Draw context menu
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
    if (ImGui::BeginPopup("NodeWindowPopup"))
    {
        ImVec2 scene_pos = ImGui::GetMousePosOnOpeningCurrentPopup();

        for(int i = (int) Embedded::NodeTypes::Node_Group; i < (int) Embedded::NodeTypes::Count; i++) {
            if(ImGui::MenuItem(Node::s_nodeName[i], NULL, false)) { 
                nodeGroup->addNode((Embedded::NodeTypes)i); imnodes::SetNodeScreenSpacePos(nodeGroup->getNodes().back()->getId(), scene_pos);
            }
        }

        ImGui::EndPopup();
    }
    ImGui::PopStyleVar();
}

void NodeWindow::drawNode(std::shared_ptr<Node>& node) 
{

    if(!node)
        return;

    if (m_lastSelected.lock() != AppManager::getSelectedItem().lock())
        imnodes::SetNodeGridSpacePos(node->getId(), node->getPosition());

    // Style for nodes
    if(node->getLoopFound()) {
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
    ImGui::PushItemWidth(m_nodeWidth);
    ImGui::LabelText("###Label", "%s%s%i", node->getName().c_str(), " - ", node->getId());
    imnodes::EndNodeTitleBar();

    switch (node->getType()) {
        case Embedded::NodeTypes::Initial_Value:
            drawInitialValue(node);
            break;
        case Embedded::NodeTypes::Final_Value:
            drawFinalValue(node);
            break;
        case Embedded::NodeTypes::Node_Group:
            drawNodeGroup(node);
            break;
        case Embedded::NodeTypes::And:
            drawAnd(node);
            break;
        case Embedded::NodeTypes::Or:
            drawOr(node);
            break;
        case Embedded::NodeTypes::Two_Stage:
            drawTwoStage(node);
            break;
        case Embedded::NodeTypes::Curve:
            drawCurve(node);
            break;
        case Embedded::NodeTypes::Compare:
            drawCompare(node);
            break;
        case Embedded::NodeTypes::On_Off:
            drawOnOff(node);
            break;
        case Embedded::NodeTypes::Invert:
            drawInvert(node);
            break;
        case Embedded::NodeTypes::Toggle:
            drawToggle(node);
            break;
        case Embedded::NodeTypes::Delay:
            drawDelay(node);
            break;
        case Embedded::NodeTypes::Push_Button:
            drawPushButton(node);
            break;
        case Embedded::NodeTypes::Map_Value:
            drawMapValue(node);
            break;
        case Embedded::NodeTypes::Math:
            drawMath(node);
            break;
        case Embedded::NodeTypes::Value:
            drawValue(node);
            break;
        case Embedded::NodeTypes::Select:
            drawSelect(node);
            break;
    }

    ImGui::PopItemWidth();
    imnodes::EndNode();

    if(node->getLoopFound()) {
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
        if(!fromNode)
            continue;
        for (size_t fromLinkAttribute = 0; fromLinkAttribute < fromNode->getInputs().size(); fromLinkAttribute++) {
            auto& toNode = fromNode->getInput(fromLinkAttribute).node;

            if(!toNode.expired()) {

                if(m_mode == Mode::Simulation) {
                    float outputValue = fromNode->getInput(fromLinkAttribute).node.lock()->getOutput(fromNode->getInput(fromLinkAttribute).outputIndex);
                    if(outputValue > 0.0f) {
                        imnodes::PushColorStyle(imnodes::ColorStyle_Link, IM_COL32(20, 200, 20, 255));
                    } else {
                        //imnodes::PushColorStyle(imnodes::ColorStyle_Link, IM_COL32(200, 20, 20, 255));
                        imnodes::PushColorStyle(imnodes::ColorStyle_Link, grayColor);
                    }
                }

                imnodes::Link(
                    (fromNode->getId() << 16) + (toNode.lock()->getId() << 8) + fromLinkAttribute,
                    (toNode.lock()->getId() << 8) + fromNode->getInput(fromLinkAttribute).outputIndex,
                    (fromNode->getId() << 8) + fromLinkAttribute + fromNode->getOutputs().size());

                if(m_mode == Mode::Simulation) {
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
            nodeGroup->getNode(endNodeIndex)->getInput(endAttributeIndex - nodeGroup->getNode(endNodeIndex)->getOutputs().size()) = {nodeGroup->getNode(beginNodeIndex), beginAttributeIndex };
            nodeGroup->sortNodes();
            //std::cout << "saving output to input\n";
            return;
        }

        //Then start node is an input
        //Make sure the end node is an output
        if (endAttributeIndex >= nodeGroup->getNode(endNodeIndex)->getOutputs().size())
            return;
        nodeGroup->getNode(beginNodeIndex)->getInput(beginAttributeIndex) = {nodeGroup->getNode(endNodeIndex), endAttributeIndex };
        nodeGroup->sortNodes();
    }
}

void NodeWindow::isLinkDeleted(std::shared_ptr<NodeGroup>& nodeGroup)
{
    const int num_selected = imnodes::NumSelectedLinks();
    if(num_selected > 0 && ImGui::IsKeyReleased(ImGui::GetKeyIndex(ImGuiKey_Delete))) {
        static std::vector<int> selected_links;
        selected_links.resize(static_cast<size_t>(num_selected));
        imnodes::GetSelectedLinks(selected_links.data());
        for(const int link_id : selected_links) {
            int nodeId = (link_id >> 16) & 0xFF;
            int linkAttribute = link_id & 0xFF;

            nodeGroup->getNode(nodeId)->getInput(linkAttribute).node.reset();
        }

        if(nodeGroup)
            nodeGroup->sortNodes();
    }
}

void NodeWindow::isNodeDeleted(std::shared_ptr<NodeGroup>& nodeGroup)
{
    const int num_selected = imnodes::NumSelectedNodes();
    if(num_selected > 0 && ImGui::IsKeyReleased(ImGui::GetKeyIndex(ImGuiKey_Delete))) {
        static std::vector<int> selected_nodes;
        selected_nodes.resize(static_cast<size_t>(num_selected));
        imnodes::GetSelectedNodes(selected_nodes.data());
        for(const int node_id : selected_nodes) {
            
            if(!nodeGroup)
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

    for(auto n : nodeGroup->getNodes()) {
        n->getPosition() = imnodes::GetNodeGridSpacePos(n->getId());
    }
    
}

void NodeWindow::doValuePopup(std::shared_ptr<NodeGroup>& nodeGroup)
{
    int hovered;
    if(m_mode == Mode::Simulation && imnodes::IsPinHovered(&hovered))
        ImGui::SetTooltip("%.2f", nodeGroup->getValue(hovered));
}

imnodes::EditorContext* NodeWindow::getContext(std::shared_ptr<NodeGroup>& nodeGroup) 
{

    if(m_contexts.find(nodeGroup) == m_contexts.end())
        m_contexts[nodeGroup] = imnodes::EditorContextCreate();
        
    return m_contexts[nodeGroup];
}

void NodeWindow::drawFinalValue(std::shared_ptr<Node>& node) 
{
    imnodes::BeginInputAttribute(node->getIntputId(0));
    ImGui::Text("Final Value");
    imnodes::EndInputAttribute();

    if(m_mode == Mode::Simulation) {

        bool onOff = false;
        bool floatValue = false;
        bool percentValue = false;

        if(!m_nodeGroup.expired()) {
            auto nodeGroup = m_nodeGroup.lock();
            auto type = nodeGroup->getType();
            switch(type) {
                case IOTypes::Types::Output_12V_Pwm:
                    percentValue = true;
                    break;
                case IOTypes::Types::Output_12V:
                case IOTypes::Types::Output_12V_Low_Current:
                case IOTypes::Types::Output_Ground:
                case IOTypes::Types::Input_12V:
                case IOTypes::Types::Input_Ground:
                case IOTypes::Types::Input_5V:
                    onOff = true;
                    break;
                case IOTypes::Types::Input_5V_Variable:
                case IOTypes::Types::Input_Can:
                    floatValue = true;
                    break;
            }
        }

        float value = 0.0f;
        if(!node->getInput(0).node.expired())
            value = node->getInput(0).node.lock()->getOutputValue(node->getInput(0).outputIndex);

        if(onOff) {
            const char* text;
            ImU32 color;
            ImU32 hoverColor;

            if(value > 0.0f) {
                text = "On";
                color = IM_COL32(20, 200, 20, 255);
                hoverColor = IM_COL32(20, 220, 20, 255);
            } else {
                text = "Off";
                color = IM_COL32(200, 20, 20, 255);
                hoverColor = IM_COL32(220, 20, 20, 255);
            }
            ImGui::PushStyleColor(ImGuiCol_Button, color);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, hoverColor);
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, hoverColor);
            ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.0f);
            ImGui::Button(text, ImVec2(m_nodeWidth, 0.0f));
            ImGui::PopStyleVar();
            ImGui::PopStyleColor(3);
        } else if(floatValue) {
            ImGui::Text("%4.2f", value);
        } else if(percentValue) {
            ImGui::ProgressBar(value / 100.0f, ImVec2(m_nodeWidth, 0.0f));
        }

    }
}

void NodeWindow::drawInitialValue(std::shared_ptr<Node>& node) 
{
    if(m_mode == Mode::Simulation) {
        imnodes::BeginStaticAttribute(node->getValueId(0));
        ImGui::DragFloat("###float1", &node->getValue(0), 0.01f, 0.0f, 5.0f, "%.2f");
        imnodes::EndStaticAttribute();

        const char* text;
        ImU32 color;
        ImU32 hoverColor;
        if(node->getValue(0) > 0.0f) {
            text = "On";
            color = IM_COL32(20, 200, 20, 255);
            hoverColor = IM_COL32(20, 220, 20, 255);
        } else {
            text = "Off";
            color = IM_COL32(200, 20, 20, 255);
            hoverColor = IM_COL32(220, 20, 20, 255);
        }
        ImGui::PushStyleColor(ImGuiCol_Button, color);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, hoverColor);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, hoverColor);
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.0f);
        if(ImGui::Button(text, ImVec2(m_nodeWidth, 0.0f)))
            node->getValue(0) > 0.0f ? node->getValue(0) = 0.0f : node->getValue(0) = 1.0f;
        ImGui::PopStyleVar();
        ImGui::PopStyleColor(3);
    }

    imnodes::BeginOutputAttribute(node->getOutputId(0));
    ImGui::Indent(m_nodeWidth - ImGui::CalcTextSize("Result").x);
    ImGui::Text("Result");
    imnodes::EndOutputAttribute();
}

void NodeWindow::drawNodeGroup(std::shared_ptr<Node>& node) 
{
    imnodes::BeginStaticAttribute(node->getValueId(0));

    // Fix padding on combo box
    ImGui::PopStyleVar(2);

    std::shared_ptr<NodeGroup> thisNodeGroup;
    if(!m_nodeGroup.expired())
        thisNodeGroup = m_nodeGroup.lock();

    std::shared_ptr<NodeGroup> selectedNodeGroup = AppManager::getConfig()->findNodeGroup(node->getSelectedNodeGroup());

    if(ImGui::BeginCombo("###pinsCombo", !selectedNodeGroup ? "" : selectedNodeGroup->getName().c_str())) {

        for(auto& nodeGroup : AppManager::getConfig()->getNodeGroups()) {

            // Skip if it is this node group
            if(thisNodeGroup && thisNodeGroup == nodeGroup)
                continue;

            ImGui::PushID(nodeGroup.get());
            bool isSelected = nodeGroup == selectedNodeGroup;
            if(ImGui::Selectable(nodeGroup->getName().c_str(), isSelected)) {
                node->setSelectedNodeGroup(nodeGroup->getId());
            }
            ImGui::PopID();

            if(isSelected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    // End fix padding on combo box
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(1.f, 1.f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.f, 0.f));

    if(m_mode == Mode::Simulation) {

        ImGui::DragFloat("###float1", &node->getValue(0), 1.0f, 0.0f, 10000.0f, "%.2f");

        const char* text;
        ImU32 color;
        ImU32 hoverColor;
        if(node->getValue(0) > 0.0f) {
            text = "On";
            color = IM_COL32(20, 200, 20, 255);
            hoverColor = IM_COL32(20, 220, 20, 255);
        } else {
            text = "Off";
            color = IM_COL32(200, 20, 20, 255);
            hoverColor = IM_COL32(220, 20, 20, 255);
        }
        ImGui::PushStyleColor(ImGuiCol_Button, color);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, hoverColor);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, hoverColor);
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.0f);
        if(ImGui::Button(text, ImVec2(m_nodeWidth, 0.0f)))
            node->getValue(0) > 0.0f ? node->getValue(0) = 0.0f : node->getValue(0) = 1.0f;
        ImGui::PopStyleVar();
        ImGui::PopStyleColor(3);
        
    }

    imnodes::EndStaticAttribute();

    imnodes::BeginOutputAttribute(node->getOutputId(0));
    ImGui::Indent(m_nodeWidth - ImGui::CalcTextSize("Result").x);
    ImGui::Text("Result");
    imnodes::EndOutputAttribute();
}

void NodeWindow::drawAnd(std::shared_ptr<Node>& node)
{
    imnodes::PushColorStyle(imnodes::ColorStyle_Pin, boolColor);

    imnodes::BeginInputAttribute(node->getIntputId(0));
    ImGui::Text("Input");
    imnodes::EndInputAttribute();

    imnodes::BeginInputAttribute(node->getIntputId(1));
    ImGui::Text("Input");
    imnodes::EndInputAttribute();

    imnodes::BeginInputAttribute(node->getIntputId(2));
    ImGui::Text("Input");
    imnodes::EndInputAttribute();

    imnodes::BeginInputAttribute(node->getIntputId(3));
    ImGui::Text("Input");
    imnodes::EndInputAttribute();

    imnodes::BeginInputAttribute(node->getIntputId(4));
    ImGui::Text("Input");
    imnodes::EndInputAttribute();

    imnodes::BeginOutputAttribute(node->getOutputId(0));
    ImGui::Indent(m_nodeWidth - ImGui::CalcTextSize("Result").x);
    ImGui::Text("Result");
    imnodes::EndOutputAttribute();

    imnodes::PopColorStyle();
}

void NodeWindow::drawOr(std::shared_ptr<Node>& node) 
{
    drawAnd(node);
}

void NodeWindow::drawTwoStage(std::shared_ptr<Node>& node) 
{
    imnodes::PushColorStyle(imnodes::ColorStyle_Pin, boolColor);

    imnodes::BeginInputAttribute(node->getIntputId(0));
    ImGui::Text("Stage 1");
    imnodes::EndInputAttribute();

    imnodes::BeginStaticAttribute(node->getValueId(0));
    ImGui::DragFloat("###float1", &node->getValue(0), 0.5f, 0.0f, 100.0f, "%.0f");
    imnodes::EndStaticAttribute();

    imnodes::BeginInputAttribute(node->getIntputId(1));
    ImGui::Text("Stage 2");
    imnodes::EndInputAttribute();

    imnodes::BeginStaticAttribute(node->getValueId(1));
    ImGui::DragFloat("###float1", &node->getValue(1), 0.5f, 0.0f, 100.0f, "%.0f");
    imnodes::EndStaticAttribute();

    imnodes::PopColorStyle();

    imnodes::BeginOutputAttribute(node->getOutputId(0));
    ImGui::Indent(m_nodeWidth - ImGui::CalcTextSize("Result").x);
    ImGui::Text("Result");
    imnodes::EndOutputAttribute();
}

void NodeWindow::drawCurve(std::shared_ptr<Node>& node) 
{
    imnodes::PushColorStyle(imnodes::ColorStyle_Pin, boolColor);

    imnodes::BeginInputAttribute(node->getIntputId(0));
    ImGui::Text("Input");
    imnodes::EndInputAttribute();

    imnodes::PopColorStyle();

    imnodes::BeginStaticAttribute(node->getValueId(0));

    // Fix padding on combo box
    ImGui::PopStyleVar(2);

    int type = (int) node->getValue(0);
    if(ImGui::Combo("###Combo", &type, Node::s_curveNames, (int) Embedded::CurveNode::Types::Count))
        node->setValue(0, (float) type);

    // End fix padding on combo box
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(1.f, 1.f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.f, 0.f));

    bool repeat = node->getValue(1);
    if(ImGui::Checkbox("Repeat", &repeat))
        node->setValue(1, repeat);

    //bool onShutdown = node->getValue(2);
    //if(ImGui::Checkbox("On Shutdown", &onShutdown))
    //    node->setValue(2, onShutdown);

    ImGui::DragFloat("###float1", &node->getValue(3), 0.05f, 0.0f, 10.0f, "%.2f sec");

    imnodes::EndStaticAttribute();

    imnodes::PushColorStyle(imnodes::ColorStyle_Pin, zeroToOneColor);

    imnodes::BeginOutputAttribute(node->getOutputId(0));
    ImGui::Indent(m_nodeWidth - ImGui::CalcTextSize("Result").x);
    ImGui::Text("Result");
    imnodes::EndOutputAttribute();

    imnodes::PopColorStyle();
}

void NodeWindow::drawCompare(std::shared_ptr<Node>& node) 
{
    imnodes::BeginInputAttribute(node->getIntputId(0));
    ImGui::Text("Value 1");
    imnodes::EndInputAttribute();

    imnodes::BeginInputAttribute(node->getIntputId(1));
    ImGui::Text("Value 2");
    imnodes::EndInputAttribute();

    imnodes::BeginStaticAttribute(node->getValueId(0));
    // Fix padding on combo box
    ImGui::PopStyleVar(2);

    int type = (int)node->getValue(0);
    if(ImGui::Combo("###Combo", &type, Node::s_compareNames, (int) Embedded::CompareNode::Types::Count))
        node->setValue(0, (float)type);

    // End fix padding on combo box
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(1.f, 1.f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.f, 0.f));
    imnodes::EndStaticAttribute();

    imnodes::PushColorStyle(imnodes::ColorStyle_Pin, boolColor);

    imnodes::BeginOutputAttribute(node->getOutputId(0));
    ImGui::Indent(m_nodeWidth - ImGui::CalcTextSize("Result").x);
    ImGui::Text("Result");
    imnodes::EndOutputAttribute();

    imnodes::PopColorStyle();
}

void NodeWindow::drawOnOff(std::shared_ptr<Node>& node) 
{
    imnodes::PushColorStyle(imnodes::ColorStyle_Pin, boolColor);

    imnodes::BeginInputAttribute(node->getIntputId(0));
    ImGui::Text("On");
    imnodes::EndInputAttribute();

    imnodes::BeginInputAttribute(node->getIntputId(1));
    ImGui::Text("Off");
    imnodes::EndInputAttribute();

    imnodes::BeginOutputAttribute(node->getOutputId(0));
    ImGui::Indent(m_nodeWidth - ImGui::CalcTextSize("Result").x);
    ImGui::Text("Result");
    imnodes::EndOutputAttribute();

    imnodes::PopColorStyle();
}

void NodeWindow::drawInvert(std::shared_ptr<Node>& node) 
{
    imnodes::PushColorStyle(imnodes::ColorStyle_Pin, boolColor);

    imnodes::BeginInputAttribute(node->getIntputId(0));
    ImGui::Text("Input");
    imnodes::EndInputAttribute();

    imnodes::BeginOutputAttribute(node->getOutputId(0));
    ImGui::Indent(m_nodeWidth - ImGui::CalcTextSize("Result").x);
    ImGui::Text("Result");
    imnodes::EndOutputAttribute();

    imnodes::PopColorStyle();
}

void NodeWindow::drawToggle(std::shared_ptr<Node>& node) 
{
    imnodes::PushColorStyle(imnodes::ColorStyle_Pin, boolColor);

    imnodes::BeginInputAttribute(node->getIntputId(0));
    ImGui::Text("Input");
    imnodes::EndInputAttribute();

    imnodes::BeginOutputAttribute(node->getOutputId(0));
    ImGui::Indent(m_nodeWidth - ImGui::CalcTextSize("Result").x);
    ImGui::Text("Result");
    imnodes::EndOutputAttribute();

    imnodes::PopColorStyle();
}

void NodeWindow::drawDelay(std::shared_ptr<Node>& node) 
{
    imnodes::BeginInputAttribute(node->getIntputId(0));
    ImGui::Text("Input");
    imnodes::EndInputAttribute();

    imnodes::BeginStaticAttribute(node->getValueId(0));
    ImGui::DragFloat("###float1", &node->getValue(0), 0.05f, 0.0f, 10.0f, "%.2f sec");
    imnodes::EndStaticAttribute();

    imnodes::BeginOutputAttribute(node->getOutputId(0));
    ImGui::Indent(m_nodeWidth - ImGui::CalcTextSize("Result").x);
    ImGui::Text("Result");
    imnodes::EndOutputAttribute();
}

void NodeWindow::drawPushButton(std::shared_ptr<Node>& node) 
{
    imnodes::PushColorStyle(imnodes::ColorStyle_Pin, boolColor);

    imnodes::BeginInputAttribute(node->getIntputId(0));
    ImGui::Text("Button");
    imnodes::EndInputAttribute();

    imnodes::BeginInputAttribute(node->getIntputId(1));
    ImGui::Text("Neutral Safety");
    imnodes::EndInputAttribute();

    imnodes::BeginInputAttribute(node->getIntputId(2));
    ImGui::Text("Engine Running");
    imnodes::EndInputAttribute();

    imnodes::BeginOutputAttribute(node->getOutputId(0));
    ImGui::Indent(m_nodeWidth - ImGui::CalcTextSize("Ignition").x);
    ImGui::Text("Ignition");
    imnodes::EndOutputAttribute();

    imnodes::BeginOutputAttribute(node->getOutputId(1));
    ImGui::Indent(m_nodeWidth - ImGui::CalcTextSize("Starter").x);
    ImGui::Text("Starter");
    imnodes::EndOutputAttribute();

    imnodes::PopColorStyle();
}

void NodeWindow::drawMapValue(std::shared_ptr<Node>& node) 
{
    imnodes::BeginInputAttribute(node->getIntputId(0));
    ImGui::Text("Input");
    imnodes::EndInputAttribute();

    imnodes::BeginStaticAttribute(node->getValueId(0));
    ImGui::Text("From:");
    ImGui::DragFloat("###hidelabel1", &node->getValue(0), 1.0f, 0.0f, 0.0f, "Min:  %.2f");
    ImGui::DragFloat("###hidelabel2", &node->getValue(1), 1.0f, 0.0f, 0.0f, "Max:  %.2f");
    ImGui::Text("To:");
    ImGui::DragFloat("###hidelabel3", &node->getValue(2), 1.0f, 0.0f, 0.0f, "Min:  %.2f");
    ImGui::DragFloat("###hidelabel4", &node->getValue(3), 1.0f, 0.0f, 0.0f, "Max:  %.2f");
    imnodes::EndStaticAttribute();

    imnodes::BeginOutputAttribute(node->getOutputId(0));
    ImGui::Indent(m_nodeWidth - ImGui::CalcTextSize("Result").x);
    ImGui::Text("Result");
    imnodes::EndOutputAttribute();
}

void NodeWindow::drawMath(std::shared_ptr<Node>& node) 
{
    imnodes::BeginInputAttribute(node->getIntputId(0));
    ImGui::Text("Value 1");
    imnodes::EndInputAttribute();

    imnodes::BeginInputAttribute(node->getIntputId(1));
    ImGui::Text("Value 2");
    imnodes::EndInputAttribute();

    // Fix padding on combo box
    ImGui::PopStyleVar(2);
    imnodes::BeginStaticAttribute(node->getValueId(0));

    int type = (int)node->getValue(0);
    if(ImGui::Combo("###Combo", &type, Node::s_mathNames, (int) Embedded::MathNode::Types::Count))
        node->setValue(0, (float) type);

    // End fix padding on combo box
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(1.f, 1.f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.f, 0.f));
    imnodes::EndStaticAttribute();

    imnodes::BeginOutputAttribute(node->getOutputId(0));
    ImGui::Indent(m_nodeWidth - ImGui::CalcTextSize("Result").x);
    ImGui::Text("Result");
    imnodes::EndOutputAttribute();
}

void NodeWindow::drawValue(std::shared_ptr<Node>& node) 
{
    imnodes::BeginStaticAttribute(node->getValueId(0));
    ImGui::DragFloat("###float1", &node->getValue(0), 5.0f, 0.0f, 10000.0f, "%.2f");
    imnodes::EndStaticAttribute();


    imnodes::BeginOutputAttribute(node->getOutputId(0));
    ImGui::Indent(m_nodeWidth - ImGui::CalcTextSize("Result").x);
    ImGui::Text("Result");
    imnodes::EndOutputAttribute();
}

void NodeWindow::drawSelect(std::shared_ptr<Node>& node)
{
    imnodes::PushColorStyle(imnodes::ColorStyle_Pin, boolColor);
    imnodes::BeginInputAttribute(node->getIntputId(0));
    ImGui::Text("Selection");
    imnodes::EndInputAttribute();
    imnodes::PopColorStyle();

    imnodes::BeginInputAttribute(node->getIntputId(1));
    ImGui::Text("Input 1");
    imnodes::EndInputAttribute();

    imnodes::BeginInputAttribute(node->getIntputId(2));
    ImGui::Text("Input 2");
    imnodes::EndInputAttribute();

    imnodes::BeginOutputAttribute(node->getOutputId(0));
    ImGui::Indent(m_nodeWidth - ImGui::CalcTextSize("Result").x);
    ImGui::Text("Result");
    imnodes::EndOutputAttribute();
}
