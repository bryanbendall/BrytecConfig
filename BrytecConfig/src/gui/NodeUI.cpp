#include "NodeUI.h"

#include "AppManager.h"
#include "BrytecConfigEmbedded/Nodes/ECompareNode.h"
#include "BrytecConfigEmbedded/Nodes/ECurveNode.h"
#include "BrytecConfigEmbedded/Nodes/EMathNode.h"
#include "utils/Colors.h"
#include <IconsFontAwesome5.h>
#include <bitset>
#include <imgui_internal.h>
#include <imnodes.h>
#include <iomanip>
#include <iostream>
#include <misc/cpp/imgui_stdlib.h>
#include <sstream>

namespace UI {
static void SameHeightText(std::string text);
static void InputFloat(std::shared_ptr<Node>& node, int attribute, std::string label, int decimals = 2, float min = 0.0f, float max = 0.0f, float speed = 1.0f);
static void InputBool(std::shared_ptr<Node>& node, int attribute, std::string label);
static void ValueFloat(std::shared_ptr<Node>& node, int attribute, std::string label, float min = 0.0f, float max = 0.0f, float speed = 1.0f);
static void ValueCombo(std::shared_ptr<Node>& node, int attribute, const char* const items[], int items_count);
static void Ouput(std::shared_ptr<Node>& node, int attribute, std::string label, unsigned int color = Colors::NodeConnections::AnyValue);
static void OnOffButton(std::shared_ptr<Node>& node, float& value, bool interact);
}

void NodeUI::drawNode(std::shared_ptr<Node> node, NodeWindow::Mode& mode, std::weak_ptr<NodeGroup> nodeGroup)
{
    static bool focus = false;

    switch (node->getType()) {

    case NodeTypes::Final_Value:
        UI::InputFloat(node, 0, "Final Value");

        if (mode == NodeWindow::Mode::Simulation) {
            bool onOff = false;
            bool floatValue = false;
            bool percentValue = false;

            if (!nodeGroup.expired()) {
                auto ng = nodeGroup.lock();
                auto type = ng->getType();
                switch (type) {
                case IOTypes::Types::Output_5V:
                case IOTypes::Types::Output_Batt:
                case IOTypes::Types::Output_Ground:
                    percentValue = true;
                    break;

                case IOTypes::Types::Input_Batt:
                case IOTypes::Types::Input_Ground:
                case IOTypes::Types::Input_5V:
                    onOff = true;
                    break;
                case IOTypes::Types::Input_5V_Variable:
                case IOTypes::Types::Input_20V_Variable:
                case IOTypes::Types::Internal:
                    floatValue = true;
                    break;
                }
            }

            float& value = node->getInputValue(0);

            if (onOff) {
                UI::OnOffButton(node, value, false);
            } else if (floatValue) {
                std::stringstream stream;
                stream << std::fixed << std::setprecision(2) << value;
                UI::SameHeightText(stream.str());
            } else if (percentValue) {
                ImGui::ProgressBar(value, ImVec2(NodeWindow::nodeWidth, 0.0f));
            }
        }
        break;

    case NodeTypes::Initial_Value:
        if (mode == NodeWindow::Mode::Simulation) {
            UI::ValueFloat(node, 0, "Value", 0.0f, 5.0f, 0.01f);
            UI::OnOffButton(node, node->getValue(0), true);
        }
        UI::Ouput(node, 0, "Value");
        break;

    case NodeTypes::Node_Group:
        imnodes::BeginStaticAttribute(node->getValueId(0));

        static std::shared_ptr<NodeGroup> thisNodeGroup;

        if (!nodeGroup.expired())
            thisNodeGroup = nodeGroup.lock();

        static std::shared_ptr<NodeGroup> selectedNodeGroup;
        selectedNodeGroup = AppManager::getConfig()->findNodeGroup(node->getSelectedNodeGroup());

        if (ImGui::BeginCombo("###pinsCombo", !selectedNodeGroup ? "" : selectedNodeGroup->getName().c_str(), ImGuiComboFlags_HeightLarge)) {

            if (ImGui::IsWindowAppearing() && !focus) {
                ImGui::SetKeyboardFocusHere();
                focus = true;
            } else {
                focus = false;
            }

            ImGui::SetCursorPosX(5.0f);

            static std::string text = "";
            ImGui::InputText("###pinName", &text, ImGuiInputTextFlags_AutoSelectAll);
            ImGui::SameLine();
            ImGui::TextUnformatted(ICON_FA_SEARCH);

            ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)ImColor(0, 0, 0, 0));
            ImGui::PushStyleColor(ImGuiCol_ScrollbarBg, (ImVec4)ImColor(0, 0, 0, 0));
            ImGui::PushItemWidth(-FLT_MIN);

            ImGui::SetCursorPosX(0.0f);
            if (ImGui::BeginListBox("##listbox")) {
                for (auto& nodeGroup : AppManager::getConfig()->getNodeGroups()) {

                    // Skip if it is this node group
                    if (thisNodeGroup && thisNodeGroup == nodeGroup)
                        continue;

                    // Filter - not case sensitive
                    auto it = std::search(
                        nodeGroup->getName().begin(), nodeGroup->getName().end(),
                        text.begin(), text.end(),
                        [](unsigned char ch1, unsigned char ch2) { return std::toupper(ch1) == std::toupper(ch2); });
                    if (it == nodeGroup->getName().end())
                        continue;

                    ImGui::PushID(nodeGroup.get());
                    bool isSelected = nodeGroup == selectedNodeGroup;
                    if (ImGui::Selectable(nodeGroup->getName().c_str(), isSelected)) {
                        node->setSelectedNodeGroup(nodeGroup->getId());
                        ImGui::CloseCurrentPopup();
                    }
                    ImGui::PopID();

                    if (isSelected)
                        ImGui::SetItemDefaultFocus();
                }

                ImGui::EndListBox();
            }

            ImGui::PopStyleColor();
            ImGui::PopStyleColor();
            ImGui::PopItemWidth();

            ImGui::EndCombo();
        }
        imnodes::EndStaticAttribute();

        UI::InputFloat(node, 0, "Default Value");
        UI::InputFloat(node, 1, "Timeout", 2, 0.01f, 1000.0f, 0.01f);

        imnodes::BeginStaticAttribute(node->getValueId(1));
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

        UI::Ouput(node, 0, "Result");
        break;

    case NodeTypes::And:
        UI::InputBool(node, 0, "Input");
        UI::InputBool(node, 1, "Input");
        UI::InputBool(node, 2, "Input");
        UI::InputBool(node, 3, "Input");
        UI::InputBool(node, 4, "Input");
        UI::Ouput(node, 0, "Result", Colors::NodeConnections::Boolean);
        break;

    case NodeTypes::Or:
        UI::InputBool(node, 0, "Input");
        UI::InputBool(node, 1, "Input");
        UI::InputBool(node, 2, "Input");
        UI::InputBool(node, 3, "Input");
        UI::InputBool(node, 4, "Input");
        UI::Ouput(node, 0, "Result", Colors::NodeConnections::Boolean);
        break;

    case NodeTypes::Two_Stage:
        UI::InputBool(node, 0, "Stage 1");
        UI::InputFloat(node, 1, "Stage 1");
        UI::InputBool(node, 2, "Stage 2");
        UI::InputFloat(node, 3, "Stage 2");
        UI::Ouput(node, 0, "Result");
        break;

    case NodeTypes::Curve:
        static const char* s_curveNames[(int)CurveType::Count] = {
            "Toggle",
            "Linear",
            "Expontial",
            "Breathing"
        };
        UI::InputBool(node, 0, "Input");
        UI::ValueCombo(node, 0, s_curveNames, (int)CurveType::Count);
        UI::InputBool(node, 1, "Repeat");
        UI::InputFloat(node, 2, "Time", 2, 0.0f, 10.0f, 0.05f);
        UI::Ouput(node, 0, "Result", Colors::NodeConnections::ZeroToOne);
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
        UI::InputFloat(node, 0, "Value 1");
        UI::InputFloat(node, 1, "Value 2");
        UI::ValueCombo(node, 0, s_compareNames, (int)CompareType::Count);
        UI::Ouput(node, 0, "Result", Colors::NodeConnections::Boolean);
        break;

    case NodeTypes::On_Off:
        UI::InputBool(node, 0, "On");
        UI::InputBool(node, 1, "Off");
        UI::Ouput(node, 0, "Output", Colors::NodeConnections::Boolean);
        break;

    case NodeTypes::Invert:
        UI::InputBool(node, 0, "Input");
        UI::Ouput(node, 0, "Output", Colors::NodeConnections::Boolean);
        break;

    case NodeTypes::Toggle:
        UI::InputBool(node, 0, "Input");
        UI::Ouput(node, 0, "Output", Colors::NodeConnections::Boolean);
        break;

    case NodeTypes::Delay:
        UI::InputFloat(node, 0, "Input");
        UI::InputFloat(node, 1, "Time");
        UI::Ouput(node, 0, "Output");
        break;

    case NodeTypes::Push_Button:
        UI::InputBool(node, 0, "Button");
        UI::InputBool(node, 1, "Neutral Safety");
        UI::InputBool(node, 2, "Enging Running");
        UI::Ouput(node, 0, "Ignition", Colors::NodeConnections::Boolean);
        UI::Ouput(node, 1, "Starter", Colors::NodeConnections::Boolean);
        break;

    case NodeTypes::Map_Value:
        UI::InputFloat(node, 0, "Input");
        UI::InputFloat(node, 1, "From Min");
        UI::InputFloat(node, 2, "From Max");
        UI::InputFloat(node, 3, "To Min");
        UI::InputFloat(node, 4, "To Max");
        UI::Ouput(node, 0, "Result");
        break;

    case NodeTypes::Math:
        static const char* s_mathNames[(int)MathType::Count] = {
            "Add",
            "Subtract",
            "Multiply",
            "Divide"
        };
        UI::InputFloat(node, 0, "Value");
        UI::InputFloat(node, 1, "Value");
        UI::ValueCombo(node, 0, s_mathNames, (int)MathType::Count);
        UI::Ouput(node, 0, "Result");
        break;

    case NodeTypes::Value:
        UI::ValueFloat(node, 0, "Value");
        UI::Ouput(node, 0, "Output");
        break;

    case NodeTypes::Switch:
        UI::InputBool(node, 0, "Selection");
        UI::InputFloat(node, 1, "If True");
        UI::InputFloat(node, 2, "If False");
        UI::Ouput(node, 0, "Output");
        break;

    case NodeTypes::CanBus:
        UI::InputFloat(node, 0, "Id", 0, 0.0f, 300.0f);
        UI::Ouput(node, 0, "Data 0");
        UI::Ouput(node, 1, "Data 1");
        UI::Ouput(node, 2, "Data 2");
        UI::Ouput(node, 3, "Data 3");
        UI::Ouput(node, 4, "Data 4");
        UI::Ouput(node, 5, "Data 5");
        UI::Ouput(node, 6, "Data 6");
        UI::Ouput(node, 7, "Data 7");
        break;

    case NodeTypes::Convert:
        UI::InputFloat(node, 0, "Data 0");
        if (node->getValue(1) >= 1.0f)
            UI::InputFloat(node, 1, "Data 1");
        if (node->getValue(1) >= 2.0f) {
            UI::InputFloat(node, 2, "Data 2");
            UI::InputFloat(node, 3, "Data 3");
        }
        static const char* endians[2] = { "Big", "Little" };
        UI::ValueCombo(node, 0, endians, 2);
        static const char* outputs[3] = { "uint8", "uint16", "uint32" };
        UI::ValueCombo(node, 1, outputs, 3);
        UI::Ouput(node, 0, "Output");
        break;

    case NodeTypes::PinCurrent:
        if (mode == NodeWindow::Mode::Simulation) {
            UI::ValueFloat(node, 0, "Value", 0.0f, 30.0f, 0.1f);
        }
        UI::Ouput(node, 0, "Value");
        break;

    case NodeTypes::PID:
        UI::InputFloat(node, 0, "Input");
        UI::InputFloat(node, 1, "Target");
        UI::InputFloat(node, 2, "P");
        UI::InputFloat(node, 3, "I");
        UI::InputFloat(node, 4, "D");
        UI::Ouput(node, 0, "Value");
        break;

    case NodeTypes::Counter:
        UI::InputFloat(node, 0, "Up");
        UI::InputFloat(node, 1, "Down");
        UI::InputFloat(node, 2, "Min", 0);
        UI::InputFloat(node, 3, "Max", 0);
        UI::Ouput(node, 0, "Value");
        break;

    default:
        drawUnimplimentedNode(node);
        break;
    }
}

void NodeUI::drawUnimplimentedNode(std::shared_ptr<Node> node)
{
    UI::SameHeightText("**Unimplimented**");
}

void UI::SameHeightText(std::string text)
{
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
    ImGui::Dummy(ImVec2(1.0f, 3.0f));
    ImGui::Text(text.c_str(), "");
    ImGui::Dummy(ImVec2(1.0f, 3.0f));
    ImGui::PopStyleVar();
}

void UI::InputFloat(std::shared_ptr<Node>& node, int attribute, std::string label, int decimals, float min, float max, float speed)
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

void UI::InputBool(std::shared_ptr<Node>& node, int attribute, std::string label)
{
    imnodes::PushColorStyle(imnodes::ColorStyle_Pin, Colors::NodeConnections::Boolean);
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

void UI::ValueFloat(std::shared_ptr<Node>& node, int attribute, std::string label, float min, float max, float speed)
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

void UI::ValueCombo(std::shared_ptr<Node>& node, int attribute, const char* const items[], int items_count)
{
    imnodes::BeginStaticAttribute(node->getValueId(attribute));

    int type = (int)node->getValue(attribute);
    if (ImGui::Combo("###Combo", &type, items, items_count))
        node->setValue(attribute, (float)type);

    imnodes::EndStaticAttribute();
}

void UI::Ouput(std::shared_ptr<Node>& node, int attribute, std::string label, unsigned int color)
{
    imnodes::PushColorStyle(imnodes::ColorStyle_Pin, color);

    imnodes::BeginOutputAttribute(node->getOutputId(attribute));
    ImGui::Indent(NodeWindow::nodeWidth - ImGui::CalcTextSize(label.c_str()).x);
    SameHeightText(label);
    imnodes::EndOutputAttribute();

    imnodes::PopColorStyle();
}

void UI::OnOffButton(std::shared_ptr<Node>& node, float& value, bool interact)
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