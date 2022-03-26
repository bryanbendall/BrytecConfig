#include "NodeUI.h"

#include "gui/Nodes/AndNodeUI.h"
#include "gui/Nodes/CanBusNodeUI.h"
#include "gui/Nodes/CompareNodeUI.h"
#include "gui/Nodes/CurveNodeUI.h"
#include "gui/Nodes/DelayNodeUI.h"
#include "gui/Nodes/FinalValueNodeUI.h"
#include "gui/Nodes/InitialValueNodeUI.h"
#include "gui/Nodes/InvertNodeUI.h"
#include "gui/Nodes/MapValueNodeUI.h"
#include "gui/Nodes/MathNodeUI.h"
#include "gui/Nodes/NodeGroupNodeUI.h"
#include "gui/Nodes/OnOffNodeUI.h"
#include "gui/Nodes/OrNodeUI.h"
#include "gui/Nodes/PushButtonNodeUI.h"
#include "gui/Nodes/SwitchNodeUI.h"
#include "gui/Nodes/ToggleNodeUI.h"
#include "gui/Nodes/TwoStageNodeUI.h"
#include "gui/Nodes/ValueNodeUI.h"
#include <bitset>
#include <imnodes.h>
#include <iomanip>
#include <iostream>
#include <sstream>

void NodeUI::drawNode(std::shared_ptr<Node> node, NodeWindow::Mode& mode, std::weak_ptr<NodeGroup> nodeGroup)
{
    switch (node->getType()) {
    case NodeTypes::Final_Value:
        FinalValueNodeUI::draw(node, mode, nodeGroup);
        break;
    case NodeTypes::Initial_Value:
        InitialValueNodeUI::draw(node, mode);
        break;
    case NodeTypes::Node_Group:
        NodeGroupNodeUI::draw(node, mode, nodeGroup);
        break;
    case NodeTypes::And:
        AndNodeUI::draw(node);
        break;
    case NodeTypes::Or:
        OrNodeUI::draw(node);
        break;
    case NodeTypes::Two_Stage:
        TwoStageNodeUI::draw(node);
        break;
    case NodeTypes::Curve:
        CurveNodeUI::draw(node);
        break;
    case NodeTypes::Compare:
        CompareNodeUI::draw(node);
        break;
    case NodeTypes::On_Off:
        OnOffNodeUI::draw(node);
        break;
    case NodeTypes::Invert:
        InvertNodeUI::draw(node);
        break;
    case NodeTypes::Toggle:
        ToggleNodeUI::draw(node);
        break;
    case NodeTypes::Delay:
        DelayNodeUI::draw(node);
        break;
    case NodeTypes::Push_Button:
        PushButtonNodeUI::draw(node);
        break;
    case NodeTypes::Map_Value:
        MapValueNodeUI::draw(node);
        break;
    case NodeTypes::Math:
        MathNodeUI::draw(node);
        break;
    case NodeTypes::Value:
        ValueNodeUI::draw(node);
        break;
    case NodeTypes::Switch:
        SwitchNodeUI::draw(node);
        break;
    case NodeTypes::CanBus:
        CanBusNodeUI::draw(node);
        break;

    default:
        drawUnimplimentedNode(node);
        break;
    }
}

void NodeUI::drawUnimplimentedNode(std::shared_ptr<Node> node)
{
    SameHeightText("Unimplimented");
}

void NodeUI::SameHeightText(std::string text)
{
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
    ImGui::Dummy(ImVec2(1.0f, 3.0f));
    ImGui::Text(text.c_str(), "");
    ImGui::Dummy(ImVec2(1.0f, 3.0f));
    ImGui::PopStyleVar();
}

void NodeUI::InputFloat(std::shared_ptr<Node>& node, int attribute, std::string label, float min, float max, float speed)
{
    imnodes::BeginInputAttribute(node->getIntputId(attribute));

    if (node->getInput(attribute).ConnectedNode.expired()) {
        std::stringstream stream;
        stream << std::fixed << std::setprecision(2) << node->getInput(attribute).DefaultValue;
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