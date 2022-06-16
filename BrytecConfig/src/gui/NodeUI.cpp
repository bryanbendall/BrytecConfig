#include "NodeUI.h"

#include "AppManager.h"
#include "BrytecConfigEmbedded/Nodes/ECompareNode.h"
#include "BrytecConfigEmbedded/Nodes/ECurveNode.h"
#include "BrytecConfigEmbedded/Nodes/EMathNode.h"
#include <bitset>
#include <imnodes.h>
#include <iomanip>
#include <iostream>
#include <sstream>

void NodeUI::drawNode(std::shared_ptr<Node> node, NodeWindow::Mode& mode, std::weak_ptr<NodeGroup> nodeGroup)
{
    switch (node->getType()) {

    case NodeTypes::Final_Value:
        NodeUI::InputFloat(node, 0, "Final Value");

        if (mode == NodeWindow::Mode::Simulation) {
            bool onOff = false;
            bool floatValue = false;
            bool percentValue = false;

            if (!nodeGroup.expired()) {
                auto ng = nodeGroup.lock();
                auto type = ng->getType();
                switch (type) {
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

            float& value = node->getInputValue(0);

            if (onOff) {
                NodeUI::OnOffButton(node, value, false);
            } else if (floatValue) {
                std::stringstream stream;
                stream << std::fixed << std::setprecision(2) << value;
                NodeUI::SameHeightText(stream.str());
            } else if (percentValue) {
                ImGui::ProgressBar(value / 100.0f, ImVec2(NodeWindow::nodeWidth, 0.0f));
            }
        }
        break;

    case NodeTypes::Initial_Value:
        if (mode == NodeWindow::Mode::Simulation) {
            NodeUI::ValueFloat(node, 0, "Value", 0.0f, 5.0f, 0.01f);
            NodeUI::OnOffButton(node, node->getValue(0), true);
        }
        NodeUI::Ouput(node, 0, "Value");
        break;

    case NodeTypes::Node_Group:
        imnodes::BeginStaticAttribute(node->getValueId(0));

        static std::shared_ptr<NodeGroup> thisNodeGroup;

        if (!nodeGroup.expired())
            thisNodeGroup = nodeGroup.lock();

        static std::shared_ptr<NodeGroup> selectedNodeGroup = AppManager::getConfig()->findNodeGroup(node->getSelectedNodeGroup());

        if (ImGui::BeginCombo("###pinsCombo", !selectedNodeGroup ? "" : selectedNodeGroup->getName().c_str())) {

            for (auto& nodeGroup : AppManager::getConfig()->getNodeGroups()) {

                // Skip if it is this node group
                if (thisNodeGroup && thisNodeGroup == nodeGroup)
                    continue;

                ImGui::PushID(nodeGroup.get());
                bool isSelected = nodeGroup == selectedNodeGroup;
                if (ImGui::Selectable(nodeGroup->getName().c_str(), isSelected)) {
                    node->setSelectedNodeGroup(nodeGroup->getId());
                }
                ImGui::PopID();

                if (isSelected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        if (mode == NodeWindow::Mode::Simulation) {

            ImGui::DragFloat("###float1", &node->getValue(0), 1.0f, 0.0f, 10000.0f, "%.2f");

            const char* text;
            ImU32 color;
            ImU32 hoverColor;
            if (node->getValue(0) > 0.0f) {
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
            if (ImGui::Button(text, ImVec2(NodeWindow::nodeWidth, 0.0f)))
                node->getValue(0) > 0.0f ? node->getValue(0) = 0.0f : node->getValue(0) = 1.0f;
            ImGui::PopStyleVar();
            ImGui::PopStyleColor(3);
        }

        imnodes::EndStaticAttribute();

        NodeUI::Ouput(node, 0, "Result");
        break;

    case NodeTypes::And:
        NodeUI::InputBool(node, 0, "Input");
        NodeUI::InputBool(node, 1, "Input");
        NodeUI::InputBool(node, 2, "Input");
        NodeUI::InputBool(node, 3, "Input");
        NodeUI::InputBool(node, 4, "Input");
        NodeUI::Ouput(node, 0, "Result", NodeWindow::boolColor);
        break;

    case NodeTypes::Or:
        NodeUI::InputBool(node, 0, "Input");
        NodeUI::InputBool(node, 1, "Input");
        NodeUI::InputBool(node, 2, "Input");
        NodeUI::InputBool(node, 3, "Input");
        NodeUI::InputBool(node, 4, "Input");
        NodeUI::Ouput(node, 0, "Result", NodeWindow::boolColor);
        break;

    case NodeTypes::Two_Stage:
        NodeUI::InputBool(node, 0, "Stage 1");
        NodeUI::InputFloat(node, 1, "Stage 1");
        NodeUI::InputBool(node, 2, "Stage 2");
        NodeUI::InputFloat(node, 3, "Stage 2");
        NodeUI::Ouput(node, 0, "Result");
        break;

    case NodeTypes::Curve:
        static const char* s_curveNames[(int)ECurveNode::Types::Count] = {
            "Toggle",
            "Linear",
            "Expontial",
            "Breathing"
        };
        NodeUI::InputBool(node, 0, "Input");
        NodeUI::ValueCombo(node, 0, s_curveNames, (int)ECurveNode::Types::Count);
        NodeUI::InputBool(node, 1, "Repeat");
        NodeUI::InputFloat(node, 2, "Time", 0.0f, 10.0f, 0.05f);
        NodeUI::Ouput(node, 0, "Result", NodeWindow::zeroToOneColor);
        break;

    case NodeTypes::Compare:
        static const char* s_compareNames[(int)CompareType::Count] = {
            "Equal",
            "Not Equal",
            "Greater Than",
            "Greater Equal To",
            "Less Than",
            "Less Equal To"
        };
        NodeUI::InputFloat(node, 0, "Value 1");
        NodeUI::InputFloat(node, 1, "Value 2");
        NodeUI::ValueCombo(node, 0, s_compareNames, (int)CompareType::Count);
        NodeUI::Ouput(node, 0, "Result", NodeWindow::boolColor);
        break;

    case NodeTypes::On_Off:
        NodeUI::InputBool(node, 0, "On");
        NodeUI::InputBool(node, 1, "Off");
        NodeUI::Ouput(node, 0, "Output", NodeWindow::boolColor);
        break;

    case NodeTypes::Invert:
        NodeUI::InputBool(node, 0, "Input");
        NodeUI::Ouput(node, 0, "Output", NodeWindow::boolColor);
        break;

    case NodeTypes::Toggle:
        NodeUI::InputBool(node, 0, "Input");
        NodeUI::Ouput(node, 0, "Output", NodeWindow::boolColor);
        break;

    case NodeTypes::Delay:
        NodeUI::InputFloat(node, 0, "Input");
        NodeUI::InputFloat(node, 1, "Time");
        NodeUI::Ouput(node, 0, "Output");
        break;

    case NodeTypes::Push_Button:
        NodeUI::InputBool(node, 0, "Button");
        NodeUI::InputBool(node, 1, "Neutral Safety");
        NodeUI::InputBool(node, 2, "Enging Running");
        NodeUI::Ouput(node, 0, "Ignition", NodeWindow::boolColor);
        NodeUI::Ouput(node, 1, "Starter", NodeWindow::boolColor);
        break;

    case NodeTypes::Map_Value:
        NodeUI::InputFloat(node, 0, "Input");
        NodeUI::InputFloat(node, 1, "From Min");
        NodeUI::InputFloat(node, 2, "From Max");
        NodeUI::InputFloat(node, 3, "To Min");
        NodeUI::InputFloat(node, 4, "To Max");
        NodeUI::Ouput(node, 0, "Result");
        break;

    case NodeTypes::Math:
        static const char* s_mathNames[(int)MathType::Count] = {
            "Add",
            "Subtract",
            "Multiply",
            "Divide"
        };
        NodeUI::InputFloat(node, 0, "Value");
        NodeUI::InputFloat(node, 1, "Value");
        NodeUI::ValueCombo(node, 0, s_mathNames, (int)MathType::Count);
        NodeUI::Ouput(node, 0, "Result");
        break;

    case NodeTypes::Value:
        NodeUI::ValueFloat(node, 0, "Value");
        NodeUI::Ouput(node, 0, "Output");
        break;

    case NodeTypes::Switch:
        NodeUI::InputBool(node, 0, "Selection");
        NodeUI::InputFloat(node, 1, "If True");
        NodeUI::InputFloat(node, 2, "If False");
        NodeUI::Ouput(node, 0, "Output");
        break;

    case NodeTypes::CanBus:
        NodeUI::InputFloat(node, 0, "Id", 0, 0.0f, 300.0f);
        NodeUI::Ouput(node, 0, "Data 0");
        NodeUI::Ouput(node, 1, "Data 1");
        NodeUI::Ouput(node, 2, "Data 2");
        NodeUI::Ouput(node, 3, "Data 3");
        NodeUI::Ouput(node, 4, "Data 4");
        NodeUI::Ouput(node, 5, "Data 5");
        NodeUI::Ouput(node, 6, "Data 6");
        NodeUI::Ouput(node, 7, "Data 7");
        break;

    case NodeTypes::Convert:
        NodeUI::InputFloat(node, 0, "Data 0");
        if (node->getValue(1) >= 1.0f)
            NodeUI::InputFloat(node, 1, "Data 1");
        if (node->getValue(1) >= 2.0f) {
            NodeUI::InputFloat(node, 2, "Data 2");
            NodeUI::InputFloat(node, 3, "Data 3");
        }
        static const char* endians[2] = { "Big", "Little" };
        NodeUI::ValueCombo(node, 0, endians, 2);
        static const char* outputs[3] = { "uint8", "uint16", "uint32" };
        NodeUI::ValueCombo(node, 1, outputs, 3);
        NodeUI::Ouput(node, 0, "Output");
        break;

    default:
        drawUnimplimentedNode(node);
        break;
    }
}

void NodeUI::drawUnimplimentedNode(std::shared_ptr<Node> node)
{
    SameHeightText("**Unimplimented**");
}

void NodeUI::SameHeightText(std::string text)
{
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
    ImGui::Dummy(ImVec2(1.0f, 3.0f));
    ImGui::Text(text.c_str(), "");
    ImGui::Dummy(ImVec2(1.0f, 3.0f));
    ImGui::PopStyleVar();
}

void NodeUI::InputFloat(std::shared_ptr<Node>& node, int attribute, std::string label, int decimals, float min, float max, float speed)
{
    imnodes::BeginInputAttribute(node->getIntputId(attribute));

    if (node->getInput(attribute).ConnectedNode.expired()) {
        std::stringstream stream;
        stream << std::fixed << std::setprecision(decimals) << node->getInput(attribute).DefaultValue;
        float floatX = ImGui::CalcTextSize(stream.str().c_str()).x;

        float labelX = ImGui::CalcTextSize(label.c_str()).x;
        float spaceX = ImGui::CalcTextSize(" ").x;
        float numSpaces = ((124.0f - (labelX + floatX)) / spaceX);

        for (int i = 0; i < numSpaces; i++)
            label += " ";

        label += stream.str();

        ImGui::DragFloat("###DragFloat", &node->getInput(attribute).DefaultValue, speed, min, max, label.c_str());
    } else {
        SameHeightText(label);
    }

    imnodes::EndInputAttribute();
}

void NodeUI::InputBool(std::shared_ptr<Node>& node, int attribute, std::string label)
{
    imnodes::PushColorStyle(imnodes::ColorStyle_Pin, NodeWindow::boolColor);
    imnodes::BeginInputAttribute(node->getIntputId(attribute));

    if (node->getInput(attribute).ConnectedNode.expired()) {
        bool value = node->getInput(attribute).DefaultValue;
        if (ImGui::Checkbox("###Check", &value))
            node->getInput(attribute).DefaultValue = value;
        ImGui::SameLine();
        ImGui::Text(label.c_str(), "");
    } else {
        SameHeightText(label);
    }

    imnodes::EndInputAttribute();
    imnodes::PopColorStyle();
}

void NodeUI::ValueFloat(std::shared_ptr<Node>& node, int attribute, std::string label, float min, float max, float speed)
{
    imnodes::BeginStaticAttribute(node->getValueId(attribute));

    std::stringstream stream;
    stream << std::fixed << std::setprecision(2) << node->getValue(attribute);
    float floatX = ImGui::CalcTextSize(stream.str().c_str()).x;

    float labelX = ImGui::CalcTextSize(label.c_str()).x;
    float spaceX = ImGui::CalcTextSize(" ").x;
    float numSpaces = ((124.0f - (labelX + floatX)) / spaceX);

    for (int i = 0; i < numSpaces; i++)
        label += " ";

    label += stream.str();

    ImGui::DragFloat("###DragFloat", &node->getValue(attribute), speed, min, max, label.c_str());
    imnodes::EndStaticAttribute();
}

void NodeUI::ValueCombo(std::shared_ptr<Node>& node, int attribute, const char* const items[], int items_count)
{
    imnodes::BeginStaticAttribute(node->getValueId(attribute));

    int type = (int)node->getValue(attribute);
    if (ImGui::Combo("###Combo", &type, items, items_count))
        node->setValue(attribute, (float)type);

    imnodes::EndStaticAttribute();
}

void NodeUI::Ouput(std::shared_ptr<Node>& node, int attribute, std::string label, unsigned int color)
{
    imnodes::PushColorStyle(imnodes::ColorStyle_Pin, color);

    imnodes::BeginOutputAttribute(node->getOutputId(attribute));
    ImGui::Indent(NodeWindow::nodeWidth - ImGui::CalcTextSize(label.c_str()).x);
    SameHeightText(label);
    imnodes::EndOutputAttribute();

    imnodes::PopColorStyle();
}

void NodeUI::OnOffButton(std::shared_ptr<Node>& node, float& value, bool interact)
{
    const char* text;
    ImU32 color;
    ImU32 hoverColor;
    if (value > 0.0f) {
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
    if (ImGui::Button(text, ImVec2(NodeWindow::nodeWidth, 0.0f)) && interact)
        value > 0.0f ? value = 0.0f : value = 1.0f;
    ImGui::PopStyleVar();
    ImGui::PopStyleColor(3);
}