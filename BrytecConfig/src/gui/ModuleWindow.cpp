#include "ModuleWindow.h"
#include "AppManager.h"
#include "BrytecConfigEmbedded/Deserializer/BinaryArrayDeserializer.h"
#include "BrytecConfigEmbedded/EBrytecApp.h"
#include "NotificationWindow.h"
#include "utils/Colors.h"
#include "utils/FileDialogs.h"
#include "utils/ModuleSerializer.h"
#include <IconsFontAwesome5.h>
#include <filesystem>
#include <fstream>
#include <iostream>

namespace Brytec {

static bool shouldUpdateInternalPins = false;

ModuleWindow::ModuleWindow()
{
}

void ModuleWindow::drawWindow()
{
    if (!m_opened)
        return;
    ImGui::Begin(ICON_FA_HDD " Modules", &m_opened, ImGuiWindowFlags_MenuBar);
    drawMenubar();
    if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(0) && !ImGui::IsAnyItemHovered())
        AppManager::setSelected(std::weak_ptr<Selectable>());
    drawModules();
    ImGui::End();
}

void ModuleWindow::drawMenubar()
{
    if (ImGui::BeginMenuBar()) {

        // Selected variables
        std::shared_ptr<Module> module = AppManager::getSelected<Module>();
        bool moduleSelected = module != nullptr;

        // Add
        if (ImGui::BeginMenu(ICON_FA_PLUS_CIRCLE)) {
            auto moduleList = ModuleSerializer::readModulesFromDisk();
            for (auto& modulePath : moduleList) {
                if (ImGui::MenuItem(modulePath.stem().string().c_str()))
                    AppManager::getConfig()->addModule(modulePath);
            }
            ImGui::EndMenu();
        }

        // Open
        if (ImGui::MenuItem(ICON_FA_FOLDER_OPEN)) {
            auto modulePath = FileDialogs::OpenFile("btmodule", AppManager::getModulesPath());
            if (!modulePath.empty())
                AppManager::getConfig()->addModule(modulePath);
        }

        ImGui::TextDisabled("|");

        // Filter
        ImGui::Text(ICON_FA_FILTER " Filter");
        ImGui::SetNextItemWidth(110);
        static const char* items[] = { "All", "Assigned", "Unassigned" };
        ImGui::Combo("##Filter", (int*)&m_filter, items, 3);

        ImGui::TextDisabled("|");

        // Save
        if (ImGui::MenuItem(ICON_FA_SAVE, NULL, false, moduleSelected)) {
            if (module) {

                auto path = FileDialogs::SaveFile("h");
                if (!path.empty()) {

                    std::ofstream fout(path);

                    ModuleSerializer moduleSer(module);
                    BinarySerializer ser = moduleSer.serializeBinary();

                    fout << "const uint8_t progmem_data[] PROGMEM = {" << std::endl;
                    for (auto d : ser.getData()) {
                        fout << std::showbase << std::hex << (int)d << ",";
                    }
                    fout << std::endl
                         << "};" << std::endl;

                    fout.close();

                    NotificationWindow::add({ "Saved module - " + module->getName(), NotificationType::Success });
                }
            }
        }

        // Start Simulation
        if (!m_simulateModule) {
            if (ImGui::MenuItem(ICON_FA_PLAY, NULL, false, moduleSelected)) {
                if (module) {
                m_simulateModule = false;

                ModuleSerializer moduleSer(module);
                BinarySerializer ser = moduleSer.serializeBinary();

                BrytecBoard::updateConfig(ser.getData().data(), ser.getData().size(), 0);

                EBrytecApp::deserializeModule();

                if (EBrytecApp::isDeserializeOk()) {
                        std::cout << "Deserialize completed succesfully" << std::endl;
                        EBrytecApp::setMode(EBrytecApp::Mode::Normal);
                        std::cout << "Done module setup" << std::endl;
                        m_simulateModule = true;
                } else {
                        std::cout << "Deserialize failed!!!!" << std::endl;
                }
                }
            }

        } else {
            // Stop simualation
            if (ImGui::MenuItem(ICON_FA_STOP, NULL, false, m_simulateModule))
                m_simulateModule = false;
        }

        // Do simulation, should be moved
        if (m_simulateModule) {
            float deltaTime = ImGui::GetIO().DeltaTime;
            EBrytecApp::update(deltaTime);
            // checkCan();
        }

        ImGui::EndMenuBar();
    }
}

void ModuleWindow::drawModules()
{
    uint32_t moduleCount = AppManager::getConfig()->getModules().size();
    float nextSizeX = 150;
    ImGuiStyle& style = ImGui::GetStyle();
    float window_visible_x2 = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;

    //ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.0f);
    for (uint32_t n = 0; n < moduleCount; n++) {
        ImGui::PushID(n);
        std::shared_ptr<Module> m = AppManager::getConfig()->getModules()[n];
        drawModule(m);
        //ImGui::Button("i/o", ImVec2(150, 25));
        float last_button_x2 = ImGui::GetItemRectMax().x;
        float next_button_x2 = last_button_x2 + style.ItemSpacing.x + nextSizeX; // Expected position if next button was on same line
        if (n + 1 < moduleCount && next_button_x2 < window_visible_x2)
            ImGui::SameLine();
        ImGui::PopID();
    }
    //ImGui::PopStyleVar();
}

void ModuleWindow::drawModule(std::shared_ptr<Module>& m)
{
    uint32_t numPins = m->getPhysicalPins().size();

    bool selected = AppManager::isSelected(m);

    // Split output to draw background after
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    drawList->ChannelsSplit(2);
    drawList->ChannelsSetCurrent(1);

    // Module style setup
    ImGui::BeginGroup();
    if (!m->getEnabled())
        ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyle().Colors[ImGuiCol_TextDisabled]);
    ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(0, 0, 0, 0));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(0, 0, 0, 0));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, IM_COL32(0, 0, 0, 0));

    bool titleHovered = false;
    ImVec2 label_size = ImGui::CalcTextSize("Module Default Size Size");
    float titleBarHeight = label_size.y + ImGui::GetStyle().FramePadding.y * 2.0f;

    // Draw Module button
    if (ImGui::Button(m->getName().c_str(), { label_size.x, titleBarHeight }))
        AppManager::setSelected(m);
    titleHovered = ImGui::IsItemHovered();

    // Module style end
    ImGui::PopStyleColor();
    ImGui::PopStyleColor();
    ImGui::PopStyleColor();

    // Pin button style setup
    if (!m->getEnabled())
        ImGui::PopStyleColor();
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.0f);
    float indentSize = 5.0f;
    ImGui::Indent(indentSize);
    float buttonWidth = label_size.x - (indentSize * 2);

    // Draw Physical Pin buttons
    for (uint32_t i = 0; i < numPins; i++) {
        ImGui::PushID(i);
        auto pin = m->getPhysicalPins()[i];

        std::string buttonText = pin->getNodeGroup() ? pin->getNodeGroup()->getName() : pin->getPinoutName();
        drawPinButton(std::static_pointer_cast<Pin>(pin), buttonText, buttonWidth);

        ImGui::PopID();
    }

    // Draw Internal Pin buttons
    for (uint32_t j = 0; j < m->getInternalPins().size(); j++) {
        ImGui::PushID(m->getPhysicalPins().size() + j);
        auto pin = m->getInternalPins()[j];

        std::string buttonText = pin->getNodeGroup() ? pin->getNodeGroup()->getName() : "Internal";
        drawPinButton(std::static_pointer_cast<Pin>(pin), buttonText, buttonWidth);

        ImGui::PopID();
    }

    // Pin style end
    ImGui::Spacing();
    ImGui::PopStyleVar();
    ImGui::EndGroup();

    // Draw background
    ImVec2 rectMin = ImGui::GetItemRectMin();
    ImVec2 rectMax = ImGui::GetItemRectMax();

    drawList->ChannelsSetCurrent(0);
    // node base
    drawList->AddRectFilled(
        rectMin,
        rectMax,
        Colors::Module::Background,
        4);
    // title bar:
    drawList->AddRectFilled(
        rectMin,
        { rectMax.x, rectMin.y + titleBarHeight },
        titleHovered ? Colors::Module::TitleBarHovered : Colors::Module::TitleBar,
        4,
        ImDrawCornerFlags_Top);
    // outline
    drawList->AddRect(
        rectMin,
        rectMax,
        selected ? Colors::PrimarySelection : Colors::Module::Outline,
        4);

    drawList->ChannelsMerge();

    if (shouldUpdateInternalPins) {
        m->updateInternalPins();
        shouldUpdateInternalPins = false;
    }
}

void ModuleWindow::drawPinButton(std::shared_ptr<Pin> pin, const std::string& name, float width)
{
    std::shared_ptr<NodeGroup> nodeGroup = pin->getNodeGroup();

    switch (m_filter) {
    case FilterType::All:
        break;
    case FilterType::Assigned:
        if (!nodeGroup)
            return;
        break;
    case FilterType::Unassigned:
        if (nodeGroup)
            return;
        break;
    }

    ImDrawList* drawList = ImGui::GetWindowDrawList();

    bool enabled = nodeGroup ? nodeGroup->getEnabled() : false;
    if (!enabled)
        ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyle().Colors[ImGuiCol_TextDisabled]);
    if (ImGui::Button(name.c_str(), { width, 0.0f }))
        AppManager::setSelected(pin);
    if (!enabled)
        ImGui::PopStyleColor();

    if (nodeGroup) {

        if (ImGui::BeginDragDropSource()) {
            uint64_t id = nodeGroup->getId();
            ImGui::SetDragDropPayload("NodeGroup", &id, sizeof(uint64_t));

            ImGui::Text(nodeGroup->getName().c_str(), "");
            ImGui::TextDisabled("%s", IOTypes::getString(nodeGroup->getType()));

            AppManager::setDragType(nodeGroup->getType());

            ImGui::EndDragDropSource();
        }

        if (ImGui::BeginPopupContextItem()) {
            if (ImGui::MenuItem("Remove Node Group", NULL, false)) {
                pin->setNodeGroup(nullptr);
                shouldUpdateInternalPins = true;
            }
            ImGui::EndPopup();
        }
    }

    // Selection boarder
    ImVec2 rectMin = ImGui::GetItemRectMin();
    ImVec2 rectMax = ImGui::GetItemRectMax();
    ImU32 rectColor = 0;

    if (pin == AppManager::getSelected<Pin>())
        rectColor = Colors::PrimarySelection;

    if (nodeGroup && AppManager::isSelected(nodeGroup))
        rectColor = Colors::SecondarySelection;

    if (std::find(pin->getAvailableTypes().begin(), pin->getAvailableTypes().end(), AppManager::getDragType()) != pin->getAvailableTypes().end())
        rectColor = Colors::DragHighlight;

    if (rectColor)
        drawList->AddRect(rectMin, rectMax, rectColor, 4.0f);

    // Drop Node Group
    if (ImGui::BeginDragDropTarget()) {

        // Check type to do the outline for drop
        bool accepted = false;
        if (const ImGuiPayload* tempPayload = ImGui::AcceptDragDropPayload("NodeGroup", ImGuiDragDropFlags_AcceptPeekOnly)) {
            uint64_t uuid = *(uint64_t*)tempPayload->Data;
            std::shared_ptr<NodeGroup> dragNodeGroup = AppManager::getConfig()->findNodeGroup(uuid);
            if (dragNodeGroup) {
                if (std::find(pin->getAvailableTypes().begin(), pin->getAvailableTypes().end(), dragNodeGroup->getType()) != pin->getAvailableTypes().end()) {
                accepted = true;
                }
            }
        }

        // Recieve the data if it is a compatible type
        if (accepted) {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("NodeGroup")) {

                uint64_t uuid = *(uint64_t*)payload->Data;
                std::shared_ptr<NodeGroup> dragNodeGroup = AppManager::getConfig()->findNodeGroup(uuid);

                std::shared_ptr<Pin> oldPin = AppManager::getConfig()->getAssignedPin(dragNodeGroup);
                if (oldPin)
                oldPin->setNodeGroup(nullptr);

                pin->setNodeGroup(dragNodeGroup);
                shouldUpdateInternalPins = true;
            }
        }
        ImGui::EndDragDropTarget();
    }
}

}