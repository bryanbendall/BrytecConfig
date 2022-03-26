#include "FinalValueNodeUI.h"

#include "gui/NodeUI.h"
#include <imgui.h>
#include <iomanip>
#include <sstream>

void FinalValueNodeUI::draw(std::shared_ptr<Node> node, NodeWindow::Mode& mode, std::weak_ptr<NodeGroup> nodeGroup)
{
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
}