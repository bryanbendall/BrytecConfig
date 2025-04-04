#include "GraphWindow.h"

#include "AppManager.h"
#include "data/InternalPin.h"
#include "data/PhysicalPin.h"
#include <IconsFontAwesome5.h>
#include <algorithm>
#include <imgui.h>
#include <implot.h>
#include <sstream>

namespace Brytec {

GraphWindow::GraphWindow()
{
}

void GraphWindow::drawWindow()
{
    if (!m_opened)
        return;

    ImGui::SetNextWindowSize(ImVec2(500.0f, 500.0f), ImGuiCond_FirstUseEver);
    ImGui::Begin(ICON_FA_CHART_LINE " Graph", &m_opened, ImGuiWindowFlags_HorizontalScrollbar);

    bool isCommunicationOpen = AppManager::getUsbManager().isOpen() || AppManager::getNetManager().isOpen();

    std::shared_ptr<Pin> pin = AppManager::getSelected<Pin>();
    std::shared_ptr<NodeGroup> nodeGroup = nullptr;
    if (pin)
        nodeGroup = pin->getNodeGroup();

    ImGui::BeginDisabled(!isCommunicationOpen || !nodeGroup);
    if (ImGui::Button("Graph")) {
        if (nodeGroup) {
            uint8_t moduleAddr = AppManager::getConfig()->getAssignedModuleAddress(nodeGroup);
            uint16_t pinAddr = AppManager::getConfig()->getAssignedPinAddress(nodeGroup);
            AppManager::getCanBusStream().requestNodeGroupStatus(moduleAddr, pinAddr, true);
            AppManager::getCanBusStream().send([](CanBusStreamCallbackData data) { });

            addToGraphs({ moduleAddr, pinAddr });
        }
    }
    ImGui::EndDisabled();

    ImGui::SameLine();

    bool onBus = nodeGroup ? AppManager::getConfig()->getUsedOnBus(nodeGroup) : false;
    ImGui::BeginDisabled(!isCommunicationOpen || !nodeGroup);
    if (ImGui::Button("Stop Graph")) {
        if (nodeGroup) {
            uint8_t moduleAddr = AppManager::getConfig()->getAssignedModuleAddress(nodeGroup);
            uint16_t pinAddr = AppManager::getConfig()->getAssignedPinAddress(nodeGroup);

            if (!onBus) {
                AppManager::getCanBusStream().requestNodeGroupStatus(moduleAddr, pinAddr, false);
                AppManager::getCanBusStream().send([](CanBusStreamCallbackData data) { });
            }

            removeFromGraphs({ moduleAddr, pinAddr });
        }
    }
    ImGui::EndDisabled();

    static float t = 0;
    t += ImGui::GetIO().DeltaTime;
    updateBuffers(t);

    static ImPlotAxisFlags flags = ImPlotAxisFlags_NoTickLabels;
    for (auto& buff : m_buffers) {

        ImGui::PushID(&buff);
        
        std::string name = findAddressName(buff.first);

        ImPlot::PushStyleVar(ImPlotStyleVar_FitPadding, ImVec2(0.0f, 0.2f));
        if (ImPlot::BeginPlot("##Scrolling", ImVec2(-1, 150), ImPlotFlags_NoMouseText | ImPlotFlags_NoBoxSelect)) {
            ImPlot::SetupAxes(nullptr, nullptr, ImPlotAxisFlags_NoTickLabels, ImPlotAxisFlags_AutoFit);
            ImPlot::SetupAxisLimits(ImAxis_X1, t - 8.0f, t, ImGuiCond_Always);
            ImPlot::SetupAxisLimits(ImAxis_Y1, -1, 2);
            ImPlot::SetNextFillStyle(IMPLOT_AUTO_COL, 0.5f);
            ImPlot::PlotLine(name.c_str(), &buff.second.Data[0].time, &buff.second.Data[0].value, buff.second.Data.size(), 0, buff.second.Offset, 3 * sizeof(float));
            ImPlot::PlotLine("Current", &buff.second.Data[0].time, &buff.second.Data[0].current, buff.second.Data.size(), 0, buff.second.Offset, 3 * sizeof(float));

            ImPlot::EndPlot();
        }
        ImPlot::PopStyleVar();

        ImGui::PopID();
    }

    ImGui::End();
}

void GraphWindow::addToGraphs(NodeGroupAddress addr)
{
    m_addresses.insert(addr);
}

void GraphWindow::removeFromGraphs(NodeGroupAddress addr)
{
    m_addresses.erase(addr);
    m_buffers.erase(addr);
}

void GraphWindow::updateBuffers(float timestep)
{

    for (auto& pinStatus : AppManager::getCanBusStream().getNodeGroupStatuses()) {

        NodeGroupAddress ngAddr { pinStatus.moduleAddress, pinStatus.nodeGroupIndex };
        if (m_addresses.count(ngAddr) == 0)
            continue;

        m_buffers[ngAddr].AddPoint(timestep, pinStatus.value, pinStatus.current);
    }
}

std::string GraphWindow::findAddressName(NodeGroupAddress addr)
{
    std::shared_ptr<Module> module = AppManager::getConfig()->findModule(addr.moduleAddress);
    std::shared_ptr<NodeGroup> nodeGroup = nullptr;

    if (module) {
        if (addr.pinAddress < module->getPhysicalPins().size())
            nodeGroup = module->getPhysicalPins()[addr.pinAddress]->getNodeGroup();
        else if (addr.pinAddress < module->getPhysicalPins().size() + module->getInternalPins().size())
            nodeGroup = module->getInternalPins()[addr.pinAddress - module->getPhysicalPins().size()]->getNodeGroup();
    }

    if (nodeGroup)
        return nodeGroup->getName();
    else{
        std::stringstream ss;
        ss << "Module:" << (int)addr.moduleAddress << " - Node Group: " << (int)addr.pinAddress;
        return ss.str();
    }
}
}