#include "NodeGroupNodeUI.h"

#include "AppManager.h"
#include "gui/NodeUI.h"

void NodeGroupNodeUI::draw(std::shared_ptr<Node> node, NodeWindow::Mode& mode, std::weak_ptr<NodeGroup> nodeGroup)
{
    imnodes::BeginStaticAttribute(node->getValueId(0));

    std::shared_ptr<NodeGroup> thisNodeGroup;
    if (!nodeGroup.expired())
        thisNodeGroup = nodeGroup.lock();

    std::shared_ptr<NodeGroup> selectedNodeGroup = AppManager::getConfig()->findNodeGroup(node->getSelectedNodeGroup());

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
}