#include "NodeUI.h"

#include "AppManager.h"
#include "BrytecConfigEmbedded/Nodes/ECompareNode.h"
#include "BrytecConfigEmbedded/Nodes/ECurveNode.h"
#include "BrytecConfigEmbedded/Nodes/EInterpolateNode.h"
#include "BrytecConfigEmbedded/Nodes/EMathNode.h"
#include "data/PhysicalPin.h"
#include "utils/Colors.h"
#include <IconsFontAwesome5.h>
#include <bitset>
#include <imgui_internal.h>
#include <imnodes.h>
#include <iomanip>
#include <iostream>
#include <misc/cpp/imgui_stdlib.h>
#include <sstream>

namespace Brytec {

namespace UI {
static void SameHeightText(std::string text);
static void DragFloat(std::shared_ptr<Node>& node, float& value, std::string label, int decimals = 2, float min = 0.0f, float max = 0.0f, float speed = 1.0f);
static void InputOnly(std::shared_ptr<Node>& node, int attribute, std::string label);
static void InputFloat(std::shared_ptr<Node>& node, int attribute, std::string label, int decimals = 2, float min = 0.0f, float max = 0.0f, float speed = 1.0f);
static void InputBool(std::shared_ptr<Node>& node, int attribute, std::string label);
static void ValueFloat(std::shared_ptr<Node>& node, int attribute, std::string label, int decimals = 2, float min = 0.0f, float max = 0.0f, float speed = 1.0f);
static void ValueHex(std::shared_ptr<Node>& node, int attribute, std::string label, int min = 0, int max = 0, float speed = 1.0f);
static void ValueCombo(std::shared_ptr<Node>& node, int attribute, const char* const items[], int items_count);
static void Ouput(std::shared_ptr<Node>& node, int attribute, std::string label, unsigned int color = Colors::NodeConnections::AnyValue);
static void OnOffButton(std::shared_ptr<Node>& node, float& value, bool interact);
}

static float s_nodeWidth = 0.0f;

void NodeUI::drawNode(std::shared_ptr<Node> node, NodeWindow::Mode& mode, std::weak_ptr<NodeGroup> nodeGroup, float nodeWidth)
{
    s_nodeWidth = nodeWidth;

    switch (node->getType()) {

    case NodeTypes::Final_Value: {
        UI::InputFloat(node, 0, "Final Value");

        if (mode == NodeWindow::Mode::Simulation) {
            bool onOff = false;
            bool floatValue = false;
            bool percentValue = false;
            bool color = false;

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
                case IOTypes::Types::Color:
                    color = true;
                    break;
                default:
                    break;
                }
            }

            float& value = node->getInputValue(0);

            if (onOff)
                UI::OnOffButton(node, value, false);

            if (floatValue) {
                std::stringstream stream;
                stream << std::fixed << std::setprecision(2) << value;
                UI::SameHeightText(stream.str());
            }

            if (percentValue)
                ImGui::ProgressBar(value, ImVec2(nodeWidth, 0.0f));

            if (color) {
                ImVec4 vColor = ImGui::ColorConvertU32ToFloat4((uint32_t)value);
                ImGui::ColorButton("##testcol", vColor, 0, ImVec2(nodeWidth, 0));
            }
        }
        break;
    }

    case NodeTypes::Initial_Value: {
        if (mode == NodeWindow::Mode::Simulation) {
            UI::ValueFloat(node, 0, "Value", 0.0f, 5.0f, 0.01f);
            UI::OnOffButton(node, node->getValue(0), true);
        }
        UI::Ouput(node, 0, "Value");
        break;
    }

    case NodeTypes::Node_Group: {
        static bool nodeGroupSearchFocus = false;
        ImNodes::BeginStaticAttribute(node->getValueId(0));

        static std::shared_ptr<NodeGroup> thisNodeGroup;

        if (!nodeGroup.expired())
            thisNodeGroup = nodeGroup.lock();

        static std::shared_ptr<NodeGroup> selectedNodeGroup;
        selectedNodeGroup = AppManager::getConfig()->findNodeGroup(node->getSelectedNodeGroup());

        if (ImGui::BeginCombo("###pinsCombo", !selectedNodeGroup ? "" : selectedNodeGroup->getName().c_str(), ImGuiComboFlags_HeightLarge)) {

            // Auto select search box
            if (ImGui::IsWindowAppearing() && !nodeGroupSearchFocus) {
                ImGui::SetKeyboardFocusHere();
                nodeGroupSearchFocus = true;
            } else {
                nodeGroupSearchFocus = false;
            }

            // Filter search box
            static std::string text = "";
            ImGui::SetCursorPosX(5.0f);
            ImGui::InputText("###pinName", &text, ImGuiInputTextFlags_AutoSelectAll);
            ImGui::SameLine();
            ImGui::TextUnformatted(ICON_FA_SEARCH);

            ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)ImColor(0, 0, 0, 0));
            ImGui::PushStyleColor(ImGuiCol_ScrollbarBg, (ImVec4)ImColor(0, 0, 0, 0));
            ImGui::PushItemWidth(-FLT_MIN);

            ImGui::SetCursorPosX(0.0f);
            if (ImGui::BeginListBox("##listbox")) {
                for (auto& nodeGroupInstance : AppManager::getConfig()->getNodeGroups()) {

                    // Skip if it is this node group
                    if (thisNodeGroup && thisNodeGroup == nodeGroupInstance)
                        continue;

                    // Filter - not case sensitive
                    auto it = std::search(
                        nodeGroupInstance->getName().begin(), nodeGroupInstance->getName().end(),
                        text.begin(), text.end(),
                        [](unsigned char ch1, unsigned char ch2) { return std::toupper(ch1) == std::toupper(ch2); });
                    if (it == nodeGroupInstance->getName().end())
                        continue;

                    // Selectable NodeGroups
                    ImGui::PushID(nodeGroupInstance.get());
                    bool isSelected = (nodeGroupInstance == selectedNodeGroup);
                    if (ImGui::Selectable(nodeGroupInstance->getName().c_str(), isSelected)) {
                        node->setSelectedNodeGroup(nodeGroupInstance->getId());
                        ImGui::CloseCurrentPopup();
                    }
                    ImGui::PopID();

                    // Already selected NodeGroup
                    if (isSelected)
                        ImGui::SetItemDefaultFocus();

                    // Tooltip
                    if (ImGui::IsItemHovered() && GImGui->HoveredIdTimer > 0.5f) {
                        std::string moduleName;
                        std::string pinoutName;
                        std::string typeString;
                        if (nodeGroupInstance->getAssigned()) {
                            auto module = AppManager::getConfig()->getAssignedModule(nodeGroupInstance);
                            if (module)
                                moduleName = module->getName();
                            auto pin = std::dynamic_pointer_cast<PhysicalPin>(AppManager::getConfig()->getAssignedPin(nodeGroupInstance));
                            if (pin)
                                pinoutName = pin->getPinoutName();
                            typeString = IOTypes::getString(nodeGroupInstance->getType());
                            ImGui::SetTooltip("Module: %s\nPin: %s\nType: %s", moduleName.c_str(), pinoutName.c_str(), typeString.c_str());
                        } else {
                            ImGui::SetTooltip("%s", "Not Assigned");
                        }
                    }
                }

                ImGui::EndListBox();
            }

            ImGui::PopStyleColor();
            ImGui::PopStyleColor();
            ImGui::PopItemWidth();

            ImGui::EndCombo();
        }
        ImNodes::EndStaticAttribute();

        UI::InputFloat(node, 0, "Default Value");
        UI::InputFloat(node, 1, "Timeout", 2, 0.01f, 1000.0f, 0.01f);

        ImNodes::BeginStaticAttribute(node->getValueId(1));
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
            if (ImGui::Button(text, ImVec2(nodeWidth, 0.0f)))
                node->getValue(0) > 0.0f ? node->getValue(0) = 0.0f : node->getValue(0) = 1.0f;
            ImGui::PopStyleVar();
            ImGui::PopStyleColor(3);
        }

        ImNodes::EndStaticAttribute();

        UI::Ouput(node, 0, "Result");
        break;
    }

    case NodeTypes::And: {
        UI::InputOnly(node, 0, "Input");
        UI::InputOnly(node, 1, "Input");
        UI::InputOnly(node, 2, "Input");
        UI::InputOnly(node, 3, "Input");
        UI::InputOnly(node, 4, "Input");
        UI::Ouput(node, 0, "Result", Colors::NodeConnections::Boolean);
        break;
    }

    case NodeTypes::Or: {
        UI::InputOnly(node, 0, "Input");
        UI::InputOnly(node, 1, "Input");
        UI::InputOnly(node, 2, "Input");
        UI::InputOnly(node, 3, "Input");
        UI::InputOnly(node, 4, "Input");
        UI::Ouput(node, 0, "Result", Colors::NodeConnections::Boolean);
        break;
    }

    case NodeTypes::Two_Stage: {
        UI::InputBool(node, 0, "Stage 1");
        UI::InputFloat(node, 1, "Stage 1");
        UI::InputBool(node, 2, "Stage 2");
        UI::InputFloat(node, 3, "Stage 2");
        UI::Ouput(node, 0, "Result");
        break;
    }

    case NodeTypes::Curve: {
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
    }

    case NodeTypes::Compare: {
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
    }

    case NodeTypes::On_Off: {
        UI::InputBool(node, 0, "On");
        UI::InputBool(node, 1, "Off");
        UI::Ouput(node, 0, "Output", Colors::NodeConnections::Boolean);
        break;
    }

    case NodeTypes::Invert: {
        UI::InputBool(node, 0, "Input");
        UI::Ouput(node, 0, "Output", Colors::NodeConnections::Boolean);
        break;
    }

    case NodeTypes::Toggle: {
        UI::InputBool(node, 0, "Input");
        UI::Ouput(node, 0, "Output", Colors::NodeConnections::Boolean);
        break;
    }

    case NodeTypes::Delay: {
        UI::InputFloat(node, 0, "Input");
        UI::InputFloat(node, 1, "Time");
        UI::Ouput(node, 0, "Output");
        break;
    }

    case NodeTypes::Push_Button: {
        UI::InputBool(node, 0, "Button");
        UI::InputBool(node, 1, "Neutral Safety");
        UI::InputBool(node, 2, "Enging Running");
        UI::Ouput(node, 0, "Ignition", Colors::NodeConnections::Boolean);
        UI::Ouput(node, 1, "Starter", Colors::NodeConnections::Boolean);
        break;
    }

    case NodeTypes::Map_Value: {
        UI::InputFloat(node, 0, "Input");
        UI::InputFloat(node, 1, "From Min");
        UI::InputFloat(node, 2, "From Max");
        UI::InputFloat(node, 3, "To Min");
        UI::InputFloat(node, 4, "To Max");
        UI::Ouput(node, 0, "Result");
        break;
    }

    case NodeTypes::Math: {
        static const char* s_mathNames[(int)MathType::Count] = {
            "Add",
            "Subtract",
            "Multiply",
            "Divide",
            "Min",
            "Max"
        };
        UI::InputFloat(node, 0, "Value");
        UI::InputFloat(node, 1, "Value");
        UI::ValueCombo(node, 0, s_mathNames, (int)MathType::Count);
        UI::Ouput(node, 0, "Result");
        break;
    }

    case NodeTypes::Value: {
        UI::ValueFloat(node, 0, "Value");
        UI::Ouput(node, 0, "Output");
        break;
    }

    case NodeTypes::Switch: {
        UI::InputBool(node, 0, "Selection");
        UI::InputFloat(node, 1, "If True");
        UI::InputFloat(node, 2, "If False");
        UI::Ouput(node, 0, "Output");
        break;
    }

    case NodeTypes::CanBusInput: {
        UI::ValueHex(node, 0, "Id");
        static const char* canBuses[] = { "Can 0", "Can 1", "Can 2", "Can 3" };
        UI::ValueCombo(node, 1, canBuses, IM_ARRAYSIZE(canBuses));
        static const char* endians[] = { "Little Endian", "Big Endian" };
        UI::ValueCombo(node, 2, endians, IM_ARRAYSIZE(endians));
        static const char* types[] = { "Uint8", "Uint16", "Uint32", "Int8", "Int16", "Int32", "Float" };
        UI::ValueCombo(node, 3, types, IM_ARRAYSIZE(types));
        UI::ValueFloat(node, 4, "Start Byte", 0, 0.0f, 7.0f, 1.0f);
        UI::Ouput(node, 0, "Output");
        break;
    }

    case NodeTypes::Convert: {
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
    }

    case NodeTypes::PinCurrent: {
        if (mode == NodeWindow::Mode::Simulation) {
            UI::ValueFloat(node, 0, "Value", 0.0f, 30.0f, 0.1f);
        }
        UI::Ouput(node, 0, "Value");
        break;
    }

    case NodeTypes::PID: {
        UI::InputFloat(node, 0, "Input");
        UI::InputFloat(node, 1, "Target");
        UI::InputFloat(node, 2, "P");
        UI::InputFloat(node, 3, "I");
        UI::InputFloat(node, 4, "D");
        UI::Ouput(node, 0, "Value");
        break;
    }

    case NodeTypes::Counter: {
        UI::InputFloat(node, 0, "Up");
        UI::InputFloat(node, 1, "Down");
        UI::InputFloat(node, 2, "Min", 0);
        UI::InputFloat(node, 3, "Max", 0);
        UI::Ouput(node, 0, "Value");
        break;
    }

    case NodeTypes::Color: {
        UI::InputFloat(node, 0, "Red", 2, 0.0f, 1.0f, 0.01f);
        UI::InputFloat(node, 1, "Green", 2, 0.0f, 1.0f, 0.01f);
        UI::InputFloat(node, 2, "Blue", 2, 0.0f, 1.0f, 0.01f);

        ImVec4 vecCol(
            node->getInput(0).DefaultValue,
            node->getInput(1).DefaultValue,
            node->getInput(2).DefaultValue,
            1.0f);

        if (ImGui::ColorButton("##b", vecCol, 0, ImVec2(s_nodeWidth, 0.0f))) {
            // Store current color and open a picker
            ImGui::GetCurrentContext()->ColorPickerRef = vecCol;
            ImGui::OpenPopup("picker");
        }

        if (ImGui::BeginPopup("picker")) {
            if (ImGui::ColorPicker4("##picker", &vecCol.x, ImGuiColorEditFlags_NoAlpha, &ImGui::GetCurrentContext()->ColorPickerRef.x)) {
                node->getInput(0).DefaultValue = vecCol.x;
                node->getInput(1).DefaultValue = vecCol.y;
                node->getInput(2).DefaultValue = vecCol.z;
            }
            ImGui::EndPopup();
        }

        UI::Ouput(node, 0, "Value");
        break;
    }

    case NodeTypes::Racepak_Switch_Panel: {
        UI::InputBool(node, 0, "Switch 1");
        UI::InputBool(node, 1, "Switch 2");
        UI::InputBool(node, 2, "Switch 3");
        UI::InputBool(node, 3, "Switch 4");
        UI::InputBool(node, 4, "Switch 5");
        UI::InputBool(node, 5, "Switch 6");
        UI::InputBool(node, 6, "Switch 7");
        UI::InputBool(node, 7, "Switch 8");
        break;
    }

    case NodeTypes::Holley_Broadcast: {
        static const char* channels[] = {
            "RTC",
            "RPM",
            "Injector Pulse Width",
            "Duty Cycle",
            "Close Loop Comp",
            "Target AFR",
            "AFR Left",
            "AFR Left",
            "AFR Average",
            "Air Temp Enrichment",
            "Coolant Enrichment",
            "Coolant AFR Offset",
            "Aftersatart Enrichment",
            "Current Learn",
            "Close Loop Status",
            "Learn Status",
            "Fuel Economy",
            "Fuel Flow",
            "MAP RoC",
            "TPS RoC",
            "Tuning Change",
            "Estimated VE",
            "Ignition Timing",
            "Knock Retard",
            "Knock Level",
            "Spark Pad2",
            "IAC Position",
            "Idle Pad1",
            "MAP",
            "TPS",
            "MAT",
            "CTS",
            "Baro",
            "Battery",
            "Oil Pressure",
            "Fuel Pressure",
            "Pedal Position",
            "Main Rev Limit",
            "Rev Limit 1",
            "Rev Limit 2",
            "AC Kick",
            "Timing Retard 1",
            "Timing Retard 2",
            "Timing Retard 3",
            "Fan 1",
            "Fan 2",
            "Second Fuel Pump",
            "AC Shutdown",
            "TCC Lockup",
            "Sensor Warning",
            "Sensor Caution",
            "Base Fuel lb hr",
            "Base Fuel VE",
            "Base Timing",
            "Base Target AFR",
            "Base Ignition Dwell",
            "Injector End Angle",
            "ECU Log Trigger",
            "Timing vs Air",
            "Timing vs Coolant",
            "Status 1",
            "Status 2",
            "Status 3",
            "Status 4",
            "Status 5",
            "Status 6",
            "Status 7",
            "Status 8",
            "Injector 1 PPH",
            "Injector 2 PPH",
            "Injector 3 PPH",
            "Injector 4 PPH",
            "Injector 5 PPH",
            "Injector 6 PPH",
            "Injector 7 PPH",
            "Injector 8 PPH",
            "Injector 9 PPH",
            "Injector 10 PPH",
            "Injector 11 PPH",
            "Injector 12 PPH",
            "Injector 1 PW",
            "Injector 2 PW",
            "Injector 3 PW",
            "Injector 4 PW",
            "Injector 5 PW",
            "Injector 6 PW",
            "Injector 7 PW",
            "Injector 8 PW",
            "Injector 9 PW",
            "Injector 10 PW",
            "Injector 11 PW",
            "Injector 12 PW",
            "Cyl 1 Fuel Cor",
            "Cyl 2 Fuel Cor",
            "Cyl 3 Fuel Cor",
            "Cyl 4 Fuel Cor",
            "Cyl 5 Fuel Cor",
            "Cyl 6 Fuel Cor",
            "Cyl 7 Fuel Cor",
            "Cyl 8 Fuel Cor",
            "Cyl 9 Fuel Cor",
            "Cyl 10 Fuel Cor",
            "Cyl 11 Fuel Cor",
            "Cyl 12 Fuel Cor",
            "Cyl 13 Fuel Cor",
            "Cyl 14 Fuel Cor",
            "Cyl 15 Fuel Cor",
            "Cyl 16 Fuel Cor",
            "Cyl 1 Timing Cor",
            "Cyl 2 Timing Cor",
            "Cyl 3 Timing Cor",
            "Cyl 4 Timing Cor",
            "Cyl 5 Timing Cor",
            "Cyl 6 Timing Cor",
            "Cyl 7 Timing Cor",
            "Cyl 8 Timing Cor",
            "Cyl 9 Timing Cor",
            "Cyl 10 Timing Cor",
            "Cyl 11 Timing Cor",
            "Cyl 12 Timing Cor",
            "Cyl 13 Timing Cor",
            "Cyl 14 Timing Cor",
            "Cyl 15 Timing Cor",
            "Cyl 16 Timing Cor",
            "DI Target FP",
            "Boost PSIG",
            "System Pad2",
            "System Pad3",
            "System Pad4",
            "Boost Gear",
            "Boost Stage",
            "Boost",
            "Boost Speed",
            "Boost Time",
            "Target Boost",
            "Trans Brake",
            "Boost Scramble Pos",
            "Boost Scramble Neg",
            "Manual Boost Build",
            "Manual Boost Reset",
            "Boost Build",
            "Boost Solenoid Duty",
            "Boost Safety",
            "Boost Master Enable",
            "Boost Man Shift Inp",
            "Boost Man Stage Inp",
            "Boost Fill Sol DC",
            "Boost Vent Sol DC",
            "WM Injection",
            "WM Manual Enable",
            "WM Low Fluid",
            "WM Pump",
            "WM Sol 1 Flow",
            "WM Sol 2 Flow",
            "WM Flow Total",
            "WM Sol 1 DC",
            "WM Sol 2 DC",
            "WM Pad1",
            "WM Pad2",
            "N20 Stage 1",
            "N20 Stage 2",
            "N20 Stage 3",
            "N20 Stage 4",
            "N20 Stage 5",
            "N20 Stage 6",
            "N20 Stage 7",
            "N20 Stage 8",
            "GPO 1",
            "GPO 2",
            "GPO 3",
            "GPO 4",
            "GPO 5",
            "GPO 6",
            "GPO 7",
            "GPO 8",
            "N20 Enabled",
            "N20 Input 1",
            "N20 Input 2",
            "N20 Input 3",
            "N20 Input 4",
            "N20 Input 5",
            "N20 Input 6",
            "N20 Input 7",
            "N20 Input 8",
            "N20 Purge",
            "N20 Lean Cutoff",
            "N20 Rich Cutoff",
            "N20 RPM Cutoff",
            "N20 MAP Cutoff",
            "N20 Purge Output",
            "N20 Dry Fuel 1",
            "N20 Dry Fuel 2",
            "N20 Dry Fuel 3",
            "N20 Dry Fuel 4",
            "N20 Dry Fuel 5",
            "N20 Dry Fuel 6",
            "N20 Dry Fuel 7",
            "N20 Dry Fuel 8",
            "N20 Timing Mod 1",
            "N20 Timing Mod 2",
            "N20 Timing Mod 3",
            "N20 Timing Mod 4",
            "N20 Timing Mod 5",
            "N20 Timing Mod 6",
            "N20 Timing Mod 7",
            "N20 Timing Mod 8",
            "N20 Timer 1",
            "N20 Timer 2",
            "N20 Timer 3",
            "N20 Timer 4",
            "N20 Timer 5",
            "N20 Timer 6",
            "N20 Timer 7",
            "N20 Timer 8",
            "N20 Pad1",
            "N20 Pad2",
            "N20 Pad3",
            "N20 Pad4",
            "Gear",
            "Speed",
            "Line Pressure",
            "Input Shaft Speed",
            "Accum Pressure",
            "TCC Duty Cycle",
            "Line Temp",
            "Torque Time",
            "Trans Man US Input",
            "Trans Man DS Input",
            "Trans Auto Man In",
            "Trans Pad1",
            "Trans Pad2",
            "TB TPS 1",
            "TB TPS 2",
            "Pedal TPS 1",
            "Pedal TPS 2",
            "TB2 TPS 1",
            "TB2 TPS 2",
            "TB Position",
            "TB2 Position",
            "Brake Pedal",
            "DBW Pad2",
            "Diag 1",
            "Diag 2",
            "Diag 3",
            "Diag 4",
            "Diag 5",
            "Diag 6",
            "Diag 7",
            "Diag 8",
            "Diag 9",
            "Diag 10",
            "Diag 11",
            "Diag 12",
            "Diag 13",
            "Diag 14",
            "Diag 15",
            "Diag 16",
            "Diag 17",
            "Diag 18",
            "Diag 19",
            "Diag 20",
            "AT Lauch Input",
            "AT Shift Input",
            "AT Manual Reset",
            "AT Gear",
            "Advanced 1D 1",
            "Advanced 1D 2",
            "Advanced 1D 3",
            "Advanced 1D 4",
            "Advanced 1D 5",
            "Advanced 1D 6",
            "Advanced 1D 7",
            "Advanced 1D 8",
            "Advanced 2D 1",
            "Advanced 2D 2",
            "Advanced 2D 3",
            "Advanced 2D 4",
            "Advanced 2D 5",
            "Advanced 2D 6",
            "Advanced 2D 7",
            "Advanced 2D 8",
            "Advanced 1D Gear 1",
            "Advanced 1D Gear 2",
            "Advanced 1D Gear 3",
            "Advanced 1D Gear 4",
            "Advanced 1D Gear 5",
            "Advanced 1D Gear 6",
            "Advanced 1D Gear 7",
            "Advanced 1D Gear 8",
            "Advanced 2D Gear 1",
            "Advanced 2D Gear 2",
            "Advanced 2D Gear 3",
            "Advanced 2D Gear 4",
            "Advanced 2D Gear 5",
            "Advanced 2D Gear 6",
            "Advanced 2D Gear 7",
            "Advanced 2D Gear 8",
            "Input 1",
            "Input 2",
            "Input 3",
            "Input 4",
            "Input 5",
            "Input 6",
            "Input 7",
            "Input 8",
            "Input 9",
            "Input 10",
            "Input 11",
            "Input 12",
            "Input 13",
            "Input 14",
            "Input 15",
            "Input 16",
            "Input 17",
            "Input 18",
            "Input 19",
            "Input 20",
            "Input 21",
            "Input 22",
            "Input 23",
            "Input 24",
            "Input 25",
            "Input 26",
            "Input 27",
            "Input 28",
            "Input 29",
            "Input 30",
            "Input 31",
            "Input 32",
            "Input 33",
            "Input 34",
            "Input 35",
            "Input 36",
            "Input 37",
            "Input 38",
            "Input 39",
            "Input 40",
            "Input 41",
            "Input 42",
            "Input 43",
            "Input 44",
            "Input 45",
            "Input 46",
            "Input 47",
            "Input 48",
            "Input 49",
            "Input 50",
            "Input 51",
            "Input 52",
            "Input 53",
            "Input 54",
            "Input 55",
            "Input 56",
            "Input 57",
            "Input 58",
            "Input 59",
            "Input 60",
            "Input 61",
            "Input 62",
            "Input 63",
            "Input 64",
            "Input 65",
            "Input 66",
            "Input 67",
            "Input 68",
            "Input 69",
            "Input 70",
            "Input 71",
            "Input 72",
            "Input 73",
            "Input 74",
            "Input 75",
            "Input 76",
            "Input 77",
            "Input 78",
            "Input 79",
            "Input 80",
            "Output 1",
            "Output 2",
            "Output 3",
            "Output 4",
            "Output 5",
            "Output 6",
            "Output 7",
            "Output 8",
            "Output 9",
            "Output 10",
            "Output 11",
            "Output 12",
            "Output 13",
            "Output 14",
            "Output 15",
            "Output 16",
            "Output 17",
            "Output 18",
            "Output 19",
            "Output 20",
            "Output 21",
            "Output 22",
            "Output 23",
            "Output 24",
            "Output 25",
            "Output 26",
            "Output 27",
            "Output 28",
            "Output 29",
            "Output 30",
            "Output 31",
            "Output 32",
            "Output 33",
            "Output 34",
            "Output 35",
            "Output 36",
            "Output 37",
            "Output 38",
            "Output 39",
            "Output 40",
            "Output 41",
            "Output 42",
            "Output 43",
            "Output 44",
            "Output 45",
            "Output 46",
            "Output 47",
            "Output 48",
            "Output 49",
            "Output 50",
            "Output 51",
            "Output 52",
            "Output 53",
            "Output 54",
            "Output 55",
            "Output 56",
            "Output 57",
            "Output 58",
            "Output 59",
            "Output 60",
            "Output 61",
            "Output 62"
        };

        static bool holleySearchFocus = false;
        ImNodes::BeginStaticAttribute(node->getValueId(0));
        int selected = (int)node->getValue(0);

        if (ImGui::BeginCombo("###pinsCombo", channels[selected], ImGuiComboFlags_HeightLarge)) {

            // Auto select search box
            if (ImGui::IsWindowAppearing() && !holleySearchFocus) {
                ImGui::SetKeyboardFocusHere();
                holleySearchFocus = true;
            } else {
                holleySearchFocus = false;
            }

            // Filter search box
            static std::string text = "";
            ImGui::SetCursorPosX(5.0f);
            ImGui::InputText("###holleyName", &text, ImGuiInputTextFlags_AutoSelectAll);
            ImGui::SameLine();
            ImGui::TextUnformatted(ICON_FA_SEARCH);

            ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)ImColor(0, 0, 0, 0));
            ImGui::PushStyleColor(ImGuiCol_ScrollbarBg, (ImVec4)ImColor(0, 0, 0, 0));
            ImGui::PushItemWidth(-FLT_MIN);

            ImGui::SetCursorPosX(0.0f);
            if (ImGui::BeginListBox("##holleyListbox")) {
                for (int i = 0; i < IM_ARRAYSIZE(channels); i++) {

                    // Filter - not case sensitive
                    std::string_view thisChannel = channels[i];
                    auto it = std::search(
                        thisChannel.begin(), thisChannel.end(),
                        text.begin(), text.end(),
                        [](unsigned char ch1, unsigned char ch2) { return std::toupper(ch1) == std::toupper(ch2); });
                    if (it == thisChannel.end())
                        continue;

                    // Selectable NodeGroups
                    ImGui::PushID(i);
                    bool isSelected = (i == selected);
                    if (ImGui::Selectable(channels[i], isSelected)) {
                        node->setValue(0, i);
                        ImGui::CloseCurrentPopup();
                    }
                    ImGui::PopID();

                    // Already selected NodeGroup
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
        ImNodes::EndStaticAttribute();

        UI::Ouput(node, 0, "Output");
        break;
    }

    case NodeTypes::Interpolate: {
        static const char* s_interpolateNames[(int)InterpolateType::Count] = {
            "Linear",
            "In Expo",
            "Out Expo",
            "In Out Expo",
            "In Cubic",
            "Out Cubic",
            "In Out Cubic"
        };
        UI::InputFloat(node, 0, "Input");
        UI::InputFloat(node, 1, "Time");
        UI::ValueCombo(node, 0, s_interpolateNames, (int)InterpolateType::Count);
        UI::Ouput(node, 0, "Output");
        break;
    }

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

void UI::DragFloat(std::shared_ptr<Node>& node, float& value, std::string label, int decimals, float min, float max, float speed)
{
    std::stringstream stream;
    stream << std::fixed << std::setprecision(decimals) << value;
    float valueWidth = ImGui::CalcTextSize(stream.str().c_str()).x;
    ImVec2 dragFloatStartPos = ImGui::GetCursorScreenPos();

    ImGui::DragFloat("###DragFloat", &value, speed, min, max, "");

    if (!ImGui::TempInputIsActive(ImGui::GetCurrentWindow()->GetID("###DragFloat"))) {

        const ImGuiStyle& style = GImGui->Style;
        ImGui::GetWindowDrawList()->AddText(dragFloatStartPos + ImVec2(style.ItemInnerSpacing.x * 2.5f, style.FramePadding.y), ImGui::GetColorU32(ImGuiCol_Text), label.c_str());

        ImVec2 valuePos = dragFloatStartPos + ImVec2(ImGui::CalcItemWidth() - style.ItemInnerSpacing.x * 2.5f - valueWidth, style.FramePadding.y);
        ImGui::GetWindowDrawList()->AddText(valuePos, ImGui::GetColorU32(ImGuiCol_Text), stream.str().c_str());
    }
}

void UI::InputOnly(std::shared_ptr<Node>& node, int attribute, std::string label)
{
    ImNodes::PushColorStyle(ImNodesCol_Pin, Colors::NodeConnections::Boolean);
    ImNodes::BeginInputAttribute(node->getIntputId(attribute));

    SameHeightText(label);

    ImNodes::EndInputAttribute();
    ImNodes::PopColorStyle();
}

void UI::InputFloat(std::shared_ptr<Node>& node, int attribute, std::string label, int decimals, float min, float max, float speed)
{
    ImNodes::BeginInputAttribute(node->getIntputId(attribute));

    if (node->getInput(attribute).ConnectedNode.expired())
        UI::DragFloat(node, node->getInput(attribute).DefaultValue, label, decimals, min, max, speed);
    else
        SameHeightText(label);

    ImNodes::EndInputAttribute();
}

void UI::InputBool(std::shared_ptr<Node>& node, int attribute, std::string label)
{
    ImNodes::PushColorStyle(ImNodesCol_Pin, Colors::NodeConnections::Boolean);
    ImNodes::BeginInputAttribute(node->getIntputId(attribute));

    if (node->getInput(attribute).ConnectedNode.expired()) {
        bool value = node->getInput(attribute).DefaultValue;
        if (ImGui::Checkbox("###Check", &value))
            node->getInput(attribute).DefaultValue = value;
        ImGui::SameLine();
        ImGui::Text(label.c_str(), "");
    } else {
        SameHeightText(label);
    }

    ImNodes::EndInputAttribute();
    ImNodes::PopColorStyle();
}

void UI::ValueFloat(std::shared_ptr<Node>& node, int attribute, std::string label, int decimals, float min, float max, float speed)
{
    ImNodes::BeginStaticAttribute(node->getValueId(attribute));

    UI::DragFloat(node, node->getValue(attribute), label, decimals, min, max, speed);

    ImNodes::EndStaticAttribute();
}

void UI::ValueHex(std::shared_ptr<Node>& node, int attribute, std::string label, int min, int max, float speed)
{
    ImNodes::BeginStaticAttribute(node->getValueId(attribute));

    int v = FloatToInt(node->getValue(attribute));

    std::stringstream stream;
    stream << "0x" << std::hex << std::uppercase << v;
    float valueWidth = ImGui::CalcTextSize(stream.str().c_str()).x;
    ImVec2 dragStartPos = ImGui::GetCursorScreenPos();

    if (ImGui::DragInt("###DragInt", &v, speed, min, max, "##0x%04X"))
        node->setValue(attribute, v);

    if (!ImGui::TempInputIsActive(ImGui::GetCurrentWindow()->GetID("###DragInt"))) {

        const ImGuiStyle& style = GImGui->Style;
        ImGui::GetWindowDrawList()->AddText(dragStartPos + ImVec2(style.ItemInnerSpacing.x * 2.5f, style.FramePadding.y), ImGui::GetColorU32(ImGuiCol_Text), label.c_str());

        ImVec2 valuePos = dragStartPos + ImVec2(ImGui::CalcItemWidth() - style.ItemInnerSpacing.x * 2.5f - valueWidth, style.FramePadding.y);
        ImGui::GetWindowDrawList()->AddText(valuePos, ImGui::GetColorU32(ImGuiCol_Text), stream.str().c_str());
    }

    ImNodes::EndStaticAttribute();
}

void UI::ValueCombo(std::shared_ptr<Node>& node, int attribute, const char* const items[], int items_count)
{
    ImNodes::BeginStaticAttribute(node->getValueId(attribute));

    int type = (int)node->getValue(attribute);
    if (ImGui::Combo("###Combo", &type, items, items_count))
        node->setValue(attribute, (float)type);

    ImNodes::EndStaticAttribute();
}

void UI::Ouput(std::shared_ptr<Node>& node, int attribute, std::string label, unsigned int color)
{
    ImNodes::PushColorStyle(ImNodesCol_Pin, color);

    ImNodes::BeginOutputAttribute(node->getOutputId(attribute));
    ImGui::Indent(s_nodeWidth - ImGui::CalcTextSize(label.c_str()).x);
    SameHeightText(label);
    ImNodes::EndOutputAttribute();

    ImNodes::PopColorStyle();
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
    if (ImGui::Button(text, ImVec2(s_nodeWidth, 0.0f)) && interact)
        value > 0.0f ? value = 0.0f : value = 1.0f;
    ImGui::PopStyleVar();
    ImGui::PopStyleColor(3);
}

}