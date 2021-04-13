#include "NodeWindow.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include "../data/Selectable.h"
#include "../AppManager.h"
#include <iostream>
#include <bitset>
#include <fontawesome/IconsFontAwesome5.h>

NodeWindow::NodeWindow() {
	imnodes::Initialize();
	imnodes::StyleColorsDark();

    imnodes::SetNodeGridSpacePos(1, ImVec2(10.0f, 10.0f));

    imnodes::Style& style = imnodes::GetStyle();
    style.colors[imnodes::ColorStyle_NodeBackground] = IM_COL32(50, 50, 50, 230);
    style.colors[imnodes::ColorStyle_TitleBarSelected] = IM_COL32(66, 150, 250, 230);
    style.colors[imnodes::ColorStyle_TitleBarHovered] = IM_COL32(53, 118, 200, 230);
    style.colors[imnodes::ColorStyle_NodeBackgroundSelected] = style.colors[imnodes::ColorStyle_NodeBackground];
    style.colors[imnodes::ColorStyle_NodeBackgroundHovered] = style.colors[imnodes::ColorStyle_NodeBackground];
}

NodeWindow::~NodeWindow() 
{
    for(auto c : m_contexts)
        imnodes::EditorContextFree(c.second);
}

void NodeWindow::drawWindow() {

    if (!m_opened)
        return;

    auto pin = std::dynamic_pointer_cast<Pin>(AppManager::getSelectedItem().lock());
    if(pin)
        imnodes::EditorContextSet(getContext(pin));

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin(ICON_FA_PROJECT_DIAGRAM" Node Editor", &m_opened);
    ImGui::PopStyleVar();

    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::SameLine();
    bool buildMode = (m_mode == Mode::Build);
    if(ImGui::Selectable("Building Mode", buildMode, 0, ImVec2(ImGui::CalcTextSize("Building Mode").x, 0))) { m_mode = Mode::Build; }
    ImGui::SameLine();
    if(ImGui::Selectable("Simulation Mode", !buildMode, 0, ImVec2(ImGui::CalcTextSize("Simulation Mode").x, 0))) { m_mode = Mode::Simulation; }
    
    imnodes::BeginNodeEditor();

    imnodes::PushStyleVar(imnodes::StyleVar_NodePaddingVertical, 5.0f);

    drawPopupMenu();
    
    if (pin) {
        for(auto n : pin->getNodes())
            drawNode(n);
    }

    addLinkData();

    imnodes::EndNodeEditor();
    
    saveNodePositions();

    isLinkCreated();
    isLinkDeleted();
    isNodeDeleted();

    doValuePopup();

    ImGui::End();

    if(pin && m_mode == Mode::Simulation)
        pin->evaluateAllNodes();
    
    m_lastSelected = AppManager::getSelectedItem();

}

void NodeWindow::drawPopupMenu() {
    auto pin = std::dynamic_pointer_cast<Pin>(AppManager::getSelectedItem().lock());
    if(!pin)
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
        ImGui::OpenPopup("context_menu");
    }

    // Draw context menu
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
    if (ImGui::BeginPopup("context_menu"))
    {
        ImVec2 scene_pos = ImGui::GetMousePosOnOpeningCurrentPopup();

        for(int i = (int)NodeTypes::Pin; i < (int)NodeTypes::Count; i++) {
            if(ImGui::MenuItem(Node::s_nodeName[i], NULL, false)) { 
                pin->addNode((NodeTypes)i); imnodes::SetNodeScreenSpacePos(pin->getNodes().back()->m_id, scene_pos); 
            }
        }

        ImGui::EndPopup();
    }
    ImGui::PopStyleVar();
}

void NodeWindow::drawNode(std::shared_ptr<Node>& node) {

    if(!node)
        return;

    if (m_lastSelected.lock() != AppManager::getSelectedItem().lock())
        imnodes::SetNodeGridSpacePos(node->m_id, node->m_position);

    if(node->m_loopFound) {
        imnodes::PushColorStyle(imnodes::ColorStyle_NodeBackground, IM_COL32(204, 25, 25, 204));
        imnodes::PushColorStyle(imnodes::ColorStyle_NodeBackgroundHovered, IM_COL32(204, 25, 25, 204));
        imnodes::PushColorStyle(imnodes::ColorStyle_NodeBackgroundSelected, IM_COL32(204, 25, 25, 204));
        imnodes::PushColorStyle(imnodes::ColorStyle_TitleBar, IM_COL32(255, 25, 25, 204));
        imnodes::PushColorStyle(imnodes::ColorStyle_TitleBarHovered, IM_COL32(255, 25, 25, 204));
        imnodes::PushColorStyle(imnodes::ColorStyle_TitleBarSelected, IM_COL32(255, 25, 25, 204));
    }

    imnodes::BeginNode(node->m_id);
    
    
    imnodes::BeginNodeTitleBar();
    ImGui::PushItemWidth(100.0f);
    ImGui::LabelText("###Label", "%s%s%i", node->m_name.c_str(), " - ", node->m_id);
    imnodes::EndNodeTitleBar();

    switch (node->m_type) {

        case NodeTypes::Output:
            drawOutput(node);
            break;
        case NodeTypes::Final_Input_Value:
            drawFinalValue(node);
            break;
        case NodeTypes::Raw_Input_Value:
            drawRawInput(node);
            break;
        case NodeTypes::Pin:
            drawPin(node);
            break;
        case NodeTypes::And:
            drawAnd(node);
            break;
        case NodeTypes::Or:
            drawOr(node);
            break;
        case NodeTypes::Two_Stage:
            drawTwoStage(node);
            break;
        case NodeTypes::Curve:
            drawCurve(node);
            break;
        case NodeTypes::Compare:
            drawCompare(node);
            break;
        case NodeTypes::On_Off:
            drawOnOff(node);
            break;
        case NodeTypes::Invert:
            drawInvert(node);
            break;
        case NodeTypes::Toggle:
            drawToggle(node);
            break;
        case NodeTypes::Delay:
            drawDelay(node);
            break;
        case NodeTypes::Push_Button:
            drawPushButton(node);
            break;
        case NodeTypes::Map_Value:
            drawMapValue(node);
            break;
        case NodeTypes::Math:
            drawMath(node);
            break;
        case NodeTypes::Value:
            drawValue(node);
            break;
    }

    ImGui::PopItemWidth();
    imnodes::EndNode();

    if(node->m_loopFound) {
        imnodes::PopColorStyle();
        imnodes::PopColorStyle();
        imnodes::PopColorStyle();
        imnodes::PopColorStyle();
        imnodes::PopColorStyle();
        imnodes::PopColorStyle();
    }
}

void NodeWindow::addLinkData() {
    auto pin = std::dynamic_pointer_cast<Pin>(AppManager::getSelectedItem().lock());
    if (!pin)
        return;

    for (size_t i = 0; i < pin->getNodes().size(); i++) {
        auto& fromNode = pin->getNodes()[i];
        if(!fromNode)
            continue;
        for (size_t fromLinkAttribute = 0; fromLinkAttribute < fromNode->m_inputs.size(); fromLinkAttribute++) {
            auto& toNode = fromNode->m_inputs[fromLinkAttribute].node;

            if(!toNode.expired()) {

                if(m_mode == Mode::Simulation) {
                    float outputValue = fromNode->m_inputs[fromLinkAttribute].node.lock()->m_outputs[fromNode->m_inputs[fromLinkAttribute].outputIndex];
                    if(outputValue > 0.0f) {
                        imnodes::PushColorStyle(imnodes::ColorStyle_Link, IM_COL32(20, 200, 20, 255));
                    } else {
                        imnodes::PushColorStyle(imnodes::ColorStyle_Link, IM_COL32(200, 20, 20, 255));
                    }
                }

                imnodes::Link(
                    (fromNode->m_id << 16) + (toNode.lock()->m_id << 8) + fromLinkAttribute,
                    (toNode.lock()->m_id << 8) + fromNode->m_inputs[fromLinkAttribute].outputIndex,
                    (fromNode->m_id << 8) + fromLinkAttribute + fromNode->m_outputs.size());

                if(m_mode == Mode::Simulation) {
                    imnodes::PopColorStyle();
                }
            }

        }
    }

}

void NodeWindow::isLinkCreated() {
    int begin, end;
    if (imnodes::IsLinkCreated(&begin, &end)) {
        unsigned char beginNodeIndex = begin >> 8;
        unsigned char beginAttributeIndex = begin & 0xFF;
        unsigned char endNodeIndex = end >> 8;
        unsigned char endAttributeIndex = end & 0xFF;
        
        //Cant link to itself
        if (beginNodeIndex == endNodeIndex)
            return;

        auto pin = std::dynamic_pointer_cast<Pin>(AppManager::getSelectedItem().lock());
        if (!pin)
            return;

        //Check if start node is an output
        if (beginAttributeIndex < pin->getNode(beginNodeIndex)->m_outputs.size()) {
            //If also an output return
            if (endAttributeIndex < pin->getNode(endNodeIndex)->m_outputs.size())
                return;
            pin->getNode(endNodeIndex)->getInput(endAttributeIndex) = { pin->getNode(beginNodeIndex), beginAttributeIndex };
            pin->sortNodes();
            //std::cout << "saving output to input\n";
            return;
        }

        //Then start node is an input
        //Make sure the end node is an output
        if (endAttributeIndex >= pin->getNode(endNodeIndex)->m_outputs.size())
            return;
        pin->getNode(beginNodeIndex)->getInput(beginAttributeIndex) = { pin->getNode(endNodeIndex), endAttributeIndex };
        pin->sortNodes();
        //std::cout << "saving input to output\n";
    }
}

void NodeWindow::isLinkDeleted() 
{
    const int num_selected = imnodes::NumSelectedLinks();
    if(num_selected > 0 && ImGui::IsKeyReleased(ImGui::GetKeyIndex(ImGuiKey_Delete))) {
        std::cout << "delete link\n";
        static std::vector<int> selected_links;
        selected_links.resize(static_cast<size_t>(num_selected));
        imnodes::GetSelectedLinks(selected_links.data());
        for(const int link_id : selected_links) {
            
        }

        auto pin = std::dynamic_pointer_cast<Pin>(AppManager::getSelectedItem().lock());
        if(!pin)
            return;
        pin->sortNodes();
    }
}

void NodeWindow::isNodeDeleted() 
{
    const int num_selected = imnodes::NumSelectedNodes();
    if(num_selected > 0 && ImGui::IsKeyReleased(ImGui::GetKeyIndex(ImGuiKey_Delete))) {
        static std::vector<int> selected_nodes;
        selected_nodes.resize(static_cast<size_t>(num_selected));
        imnodes::GetSelectedNodes(selected_nodes.data());
        for(const int node_id : selected_nodes) {
            
            auto pin = std::dynamic_pointer_cast<Pin>(AppManager::getSelectedItem().lock());
            if(!pin)
                return;

            pin->deleteNode(node_id);
            pin->sortNodes();
        }
    }
}

void NodeWindow::saveNodePositions()
{
    auto pin = std::dynamic_pointer_cast<Pin>(AppManager::getSelectedItem().lock());
    if (!pin)
        return;

    for(auto n : pin->getNodes()) {
        n->m_position = imnodes::GetNodeGridSpacePos(n->m_id);
    }
    
}

void NodeWindow::doValuePopup() 
{
    auto pin = std::dynamic_pointer_cast<Pin>(AppManager::getSelectedItem().lock());
    if(!pin)
        return;

    int hovered;
    if(m_mode == Mode::Simulation && imnodes::IsPinHovered(&hovered)) {
        ImGui::SetTooltip("%.2f", pin->getValue(hovered));

    }
}

imnodes::EditorContext* NodeWindow::getContext(std::shared_ptr<Pin>& pin) {

    if(m_contexts.find(pin) == m_contexts.end())
        m_contexts[pin] = imnodes::EditorContextCreate();
        
    return m_contexts[pin];
}

void NodeWindow::drawOutput(std::shared_ptr<Node>& node) {
    imnodes::BeginInputAttribute((node->m_id << 8) + 0);
    ImGui::Text("Pin Output");
    imnodes::EndInputAttribute();

    imnodes::BeginStaticAttribute((node->m_id << 8) + 1);
    if(m_mode == Mode::Simulation) {

        float value = !node->m_inputs[0].node.expired() ? node->m_inputs[0].node.lock()->getOutputValue(node->m_inputs[0].outputIndex) : 0.0f;
        auto pin = std::dynamic_pointer_cast<Pin>(AppManager::getSelectedItem().lock());
        
        if(pin && pin->m_pinType == PinTypes::Output_12V_Pwm)
            ImGui::ProgressBar(value / 100.0f, ImVec2(100.0f, 0.0f));

        else {
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
            ImGui::Button(text, ImVec2(100.0f, 0.0f));
            ImGui::PopStyleVar();
            ImGui::PopStyleColor(3);
        }
    }
    imnodes::EndStaticAttribute();

}

void NodeWindow::drawFinalValue(std::shared_ptr<Node>& node) {
    imnodes::BeginInputAttribute((node->m_id << 8) + 0);
    ImGui::Text("Pin Value");
    imnodes::EndInputAttribute();

    if(m_mode == Mode::Simulation) {
        float finalValue;
        if(!node->m_inputs[0].node.expired()) 
            finalValue = node->m_inputs[0].node.lock()->getOutputValue(node->m_inputs[0].outputIndex);
        else
            finalValue = 0.0f;
        ImGui::DragFloat("###finalvalue", &finalValue, 1.0f, 0.0f, 0.0f, "%.2f");
    }
}

void NodeWindow::drawRawInput(std::shared_ptr<Node>& node) {
    if(m_mode == Mode::Simulation) {
        imnodes::BeginStaticAttribute((node->m_id << 8) + 1);
        ImGui::DragFloat("###float1", &node->m_values[0], 0.01f, 0.0f, 5.0f, "%.2f");
        imnodes::EndStaticAttribute();
    }

    imnodes::BeginOutputAttribute((node->m_id << 8) + 0);
    ImGui::Indent(100.0f - ImGui::CalcTextSize("Output").x);
    ImGui::Text("Output");
    imnodes::EndOutputAttribute();
}

void NodeWindow::drawPin(std::shared_ptr<Node>& node) {
    imnodes::BeginStaticAttribute((node->m_id << 8) + 1);

    // Fix padding on combo box
    ImGui::PopStyleVar(2);

    if(ImGui::BeginCombo("###pinsCombo", node->m_pinSelecion.expired() ? "" : node->m_pinSelecion.lock()->m_name.c_str())) {
        for(auto& module : AppManager::getConfig().getModules()) {
            for(auto& pin : module->getPins()) {
                if(!pin->m_enabled)
                    continue;
                ImGui::PushID(&pin);
                bool isSelected = pin == node->m_pinSelecion.lock();
                if(ImGui::Selectable(pin->m_name.c_str(), isSelected))
                    node->m_pinSelecion = pin;
                if(ImGui::IsItemHovered()) {
                    ImGui::SetTooltip("Module: %s\nPinout: %s\nType: %s", module->m_name.c_str(),
                        pin->m_pinoutName.c_str(), (size_t)pin->m_pinType > -1 ? pin->typeNames[(size_t)pin->m_pinType] : "Not Selected");
                }
                ImGui::PopID();

                if(isSelected)
                    ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }

    // End fix padding on combo box
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(1.f, 1.f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.f, 0.f));

    if(m_mode == Mode::Simulation) {
        if(!node->m_pinSelecion.expired() && node->m_pinSelecion.lock()->m_pinType == PinTypes::Input_5V_Variable)
            ImGui::DragFloat("###float1", &node->m_values[0], 0.01f, 0.0f, 5.0f, "%.2f");

        else if(!node->m_pinSelecion.expired() && node->m_pinSelecion.lock()->m_pinType == PinTypes::Output_12V_Pwm)
            ImGui::DragFloat("###float1", &node->m_values[0], 1.0f, 0.0f, 100.0f, "%.0f");

        else {
            const char* text;
            ImU32 color;
            ImU32 hoverColor;
            if(node->m_values[0] > 0.0f) {
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
            if(ImGui::Button(text, ImVec2(100.0f, 0.0f)))
                node->m_values[0] > 0.0f ? node->m_values[0] = 0.0f : node->m_values[0] = 1.0f;
            ImGui::PopStyleVar();
            ImGui::PopStyleColor(3);
        }
    }

    imnodes::EndStaticAttribute();

    imnodes::BeginOutputAttribute((node->m_id << 8) + 0);
    ImGui::Indent(100.0f - ImGui::CalcTextSize("Output").x);
    ImGui::Text("Output");
    imnodes::EndOutputAttribute();
}

void NodeWindow::drawAnd(std::shared_ptr<Node>& node)
{
    imnodes::BeginInputAttribute((node->m_id << 8) + 1);
    ImGui::Text("Input");
    imnodes::EndInputAttribute();

    imnodes::BeginInputAttribute((node->m_id << 8) + 2);
    ImGui::Text("Input");
    imnodes::EndInputAttribute();

    imnodes::BeginInputAttribute((node->m_id << 8) + 3);
    ImGui::Text("Input");
    imnodes::EndInputAttribute();

    imnodes::BeginInputAttribute((node->m_id << 8) + 4);
    ImGui::Text("Input");
    imnodes::EndInputAttribute();

    imnodes::BeginInputAttribute((node->m_id << 8) + 5);
    ImGui::Text("Input");
    imnodes::EndInputAttribute();

    imnodes::BeginOutputAttribute((node->m_id << 8) + 0);
    ImGui::Indent(100.0f - ImGui::CalcTextSize("Output").x);
    ImGui::Text("Output");
    imnodes::EndOutputAttribute();
}

void NodeWindow::drawOr(std::shared_ptr<Node>& node) {
    drawAnd(node);
}

void NodeWindow::drawTwoStage(std::shared_ptr<Node>& node) {
    imnodes::BeginInputAttribute((node->m_id << 8) + 1);
    ImGui::Text("Stage 1");
    imnodes::EndInputAttribute();

    imnodes::BeginStaticAttribute((node->m_id << 8) + 3);
    ImGui::DragFloat("###float1", &node->m_values[0], 0.5f, 0.0f, 100.0f, "%.0f");
    imnodes::EndStaticAttribute();

    imnodes::BeginInputAttribute((node->m_id << 8) + 2);
    ImGui::Text("Stage 2");
    imnodes::EndInputAttribute();

    imnodes::BeginStaticAttribute((node->m_id << 8) + 4);
    ImGui::DragFloat("###float1", &node->m_values[1], 0.5f, 0.0f, 100.0f, "%.0f");
    imnodes::EndStaticAttribute();

    imnodes::BeginOutputAttribute((node->m_id << 8) + 0);
    ImGui::Indent(100.0f - ImGui::CalcTextSize("Output").x);
    ImGui::Text("Output");
    imnodes::EndOutputAttribute();
}

void NodeWindow::drawCurve(std::shared_ptr<Node>& node) {
    imnodes::BeginInputAttribute((node->m_id << 8) + 1);
    ImGui::Text("Input");
    imnodes::EndInputAttribute();

    imnodes::BeginStaticAttribute((node->m_id << 8) + 2);

    // Fix padding on combo box
    ImGui::PopStyleVar(2);

    int type = (int) node->m_values[0];
    if(ImGui::Combo("###Combo", &type, Node::s_curveNames, (int) CurveTypes::Count))
        node->m_values[0] = (float) type;

    // End fix padding on combo box
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(1.f, 1.f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.f, 0.f));

    bool repeat = node->m_values[1];
    if(ImGui::Checkbox("Repeat", &repeat))
        node->m_values[1] = repeat;

    bool onShutdown = node->m_values[2];
    if(ImGui::Checkbox("On Shutdown", &onShutdown))
        node->m_values[2] = onShutdown;

    ImGui::DragFloat("###float1", &node->m_values[3], 0.05f, 0.0f, 10.0f, "%.2f sec");

    imnodes::EndStaticAttribute();

    imnodes::BeginOutputAttribute((node->m_id << 8) + 0);
    ImGui::Indent(100.0f - ImGui::CalcTextSize("Output").x);
    ImGui::Text("Output");
    imnodes::EndOutputAttribute();
}

void NodeWindow::drawCompare(std::shared_ptr<Node>& node) 
{
    imnodes::BeginInputAttribute((node->m_id << 8) + 1);
    ImGui::Text("Input 1");
    imnodes::EndInputAttribute();

    imnodes::BeginStaticAttribute((node->m_id << 8) + 3);
    // Fix padding on combo box
    ImGui::PopStyleVar(2);

    int type = (int)node->m_values[0];
    if(ImGui::Combo("###Combo", &type, Node::s_compareNames, (int)CompareTypes::Count))
        node->m_values[0] = (float)type;

    // End fix padding on combo box
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(1.f, 1.f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.f, 0.f));
    imnodes::EndStaticAttribute();

    imnodes::BeginInputAttribute((node->m_id << 8) + 2);
    ImGui::Text("Input 2");
    imnodes::EndInputAttribute();

    imnodes::BeginOutputAttribute((node->m_id << 8) + 0);
    ImGui::Indent(100.0f - ImGui::CalcTextSize("Output").x);
    ImGui::Text("Output");
    imnodes::EndOutputAttribute();
}

void NodeWindow::drawOnOff(std::shared_ptr<Node>& node) {
    imnodes::BeginInputAttribute((node->m_id << 8) + 1);
    ImGui::Text("On");
    imnodes::EndInputAttribute();

    imnodes::BeginInputAttribute((node->m_id << 8) + 2);
    ImGui::Text("Off");
    imnodes::EndInputAttribute();

    imnodes::BeginOutputAttribute((node->m_id << 8) + 0);
    ImGui::Indent(100.0f - ImGui::CalcTextSize("Output").x);
    ImGui::Text("Output");
    imnodes::EndOutputAttribute();
}

void NodeWindow::drawInvert(std::shared_ptr<Node>& node) {
    imnodes::BeginInputAttribute((node->m_id << 8) + 1);
    ImGui::Text("Input");
    imnodes::EndInputAttribute();

    imnodes::BeginOutputAttribute((node->m_id << 8) + 0);
    ImGui::Indent(100.0f - ImGui::CalcTextSize("Output").x);
    ImGui::Text("Output");
    imnodes::EndOutputAttribute();
}

void NodeWindow::drawToggle(std::shared_ptr<Node>& node) {
    imnodes::BeginInputAttribute((node->m_id << 8) + 1);
    ImGui::Text("Input");
    imnodes::EndInputAttribute();

    imnodes::BeginOutputAttribute((node->m_id << 8) + 0);
    ImGui::Indent(100.0f - ImGui::CalcTextSize("Output").x);
    ImGui::Text("Output");
    imnodes::EndOutputAttribute();
}

void NodeWindow::drawDelay(std::shared_ptr<Node>& node) 
{
    imnodes::BeginInputAttribute((node->m_id << 8) + 1);
    ImGui::Text("Input");
    imnodes::EndInputAttribute();

    imnodes::BeginStaticAttribute((node->m_id << 8) + 2);
    ImGui::DragFloat("###float1", &node->m_values[0], 0.05f, 0.0f, 10.0f, "%.2f sec");
    imnodes::EndStaticAttribute();

    imnodes::BeginOutputAttribute((node->m_id << 8) + 0);
    ImGui::Indent(100.0f - ImGui::CalcTextSize("Output").x);
    ImGui::Text("Output");
    imnodes::EndOutputAttribute();
}

void NodeWindow::drawPushButton(std::shared_ptr<Node>& node) {
    imnodes::BeginInputAttribute((node->m_id << 8) + 2);
    ImGui::Text("Button");
    imnodes::EndInputAttribute();

    imnodes::BeginInputAttribute((node->m_id << 8) + 3);
    ImGui::Text("Neutral Safety");
    imnodes::EndInputAttribute();

    imnodes::BeginInputAttribute((node->m_id << 8) + 4);
    ImGui::Text("Engine Running");
    imnodes::EndInputAttribute();

    imnodes::BeginOutputAttribute((node->m_id << 8) + 0);
    ImGui::Indent(100.0f - ImGui::CalcTextSize("Ignition").x);
    ImGui::Text("Ignition");
    imnodes::EndOutputAttribute();

    imnodes::BeginOutputAttribute((node->m_id << 8) + 1);
    ImGui::Indent(100.0f - ImGui::CalcTextSize("Starter").x);
    ImGui::Text("Starter");
    imnodes::EndOutputAttribute();
}

void NodeWindow::drawMapValue(std::shared_ptr<Node>& node) {
    imnodes::BeginInputAttribute((node->m_id << 8) + 1);
    ImGui::Text("Input");
    imnodes::EndInputAttribute();

    imnodes::BeginStaticAttribute((node->m_id << 8) + 2);
    ImGui::Text("From Min");
    ImGui::DragFloat("###hidelabel1", &node->m_values[0], 1.0f, 0.0f, 0.0f, "%.2f");
    ImGui::Text("To Min");
    ImGui::DragFloat("###hidelabel2", &node->m_values[1], 1.0f, 0.0f, 0.0f, "%.2f");
    ImGui::Text("From Max");
    ImGui::DragFloat("###hidelabel3", &node->m_values[2], 1.0f, 0.0f, 0.0f, "%.2f");
    ImGui::Text("To Max");
    ImGui::DragFloat("###hidelabel4", &node->m_values[3], 1.0f, 0.0f, 0.0f, "%.2f");
    imnodes::EndStaticAttribute();

    imnodes::BeginOutputAttribute((node->m_id << 8) + 0);
    ImGui::Indent(100.0f - ImGui::CalcTextSize("Output").x);
    ImGui::Text("Output");
    imnodes::EndOutputAttribute();
}

void NodeWindow::drawMath(std::shared_ptr<Node>& node) {
    imnodes::BeginInputAttribute((node->m_id << 8) + 1);
    ImGui::Text("Input 1");
    imnodes::EndInputAttribute();

    // Fix padding on combo box
    ImGui::PopStyleVar(2);
    imnodes::BeginStaticAttribute((node->m_id << 8) + 3);

    int type = (int)node->m_values[0];
    if(ImGui::Combo("###Combo", &type, Node::s_mathNames, (int)MathTypes::Count))
        node->m_values[0] = (float) type;

    // End fix padding on combo box
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(1.f, 1.f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.f, 0.f));
    imnodes::EndStaticAttribute();

    imnodes::BeginInputAttribute((node->m_id << 8) + 2);
    ImGui::Text("Input 2");
    imnodes::EndInputAttribute();

    imnodes::BeginOutputAttribute((node->m_id << 8) + 0);
    ImGui::Indent(100.0f - ImGui::CalcTextSize("Output").x);
    ImGui::Text("Output");
    imnodes::EndOutputAttribute();
}

void NodeWindow::drawValue(std::shared_ptr<Node>& node) {
    if(m_mode == Mode::Simulation) {
        imnodes::BeginStaticAttribute((node->m_id << 8) + 1);
        ImGui::DragFloat("###float1", &node->m_values[0], 5.0f, 0.0f, 10000.0f, "%.2f");
        imnodes::EndStaticAttribute();
    }

    imnodes::BeginOutputAttribute((node->m_id << 8) + 0);
    ImGui::Indent(100.0f - ImGui::CalcTextSize("Output").x);
    ImGui::Text("Output");
    imnodes::EndOutputAttribute();
}
