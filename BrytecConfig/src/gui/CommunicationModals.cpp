#include "CommunicationModals.h"

#include "AppManager.h"
#include "BrytecConfigEmbedded/Can/EBrytecCan.h"
#include "BrytecConfigEmbedded/Deserializer/BinaryArrayDeserializer.h"
#include "data/InternalPin.h"
#include "data/PhysicalPin.h"
#include "utils/ModuleSerializer.h"
#include <imgui.h>

namespace Brytec {

void CommunicationModals::open()
{
    m_open = true;
    AppManager::getCanBusStream().getModuleStatuses().clear();
    AppManager::getCanBusStream().requestModuleStatus(CanCommands::AllModules);
    AppManager::getCanBusStream().send([](CanBusStreamCallbackData data) {});
}

void CommunicationModals::draw()
{
    if (m_open) {
        ImGui::OpenPopup("Commands", ImGuiPopupFlags_AnyPopup);
        m_open = false;
    }

    // Always center this window when appearing
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    if (ImGui::BeginPopupModal("Commands", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        drawTable();
        drawModuleCommands();
        drawSendReceive();

        drawProgressBar();
        drawCloseButton();

        ImGui::EndPopup();
    }
}

void CommunicationModals::drawTable()
{
    float bigColumn = ImGui::CalcTextSize("#####################").x;
    float smallColumn = ImGui::CalcTextSize("#######").x;
    float cellPadding = ImGui::GetStyle().CellPadding.x;
    float tableWidth = bigColumn + (smallColumn * 3) + (cellPadding * 8);

    static ImGuiTableFlags flags = ImGuiTableFlags_Sortable | ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter
        | ImGuiTableFlags_BordersV | ImGuiTableFlags_NoBordersInBody | ImGuiTableFlags_ScrollY;

    ImGui::TextUnformatted("Modules");

    if (ImGui::BeginTable("Modules", 4, flags, ImVec2(tableWidth, 300.0f))) {

        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_WidthFixed, bigColumn);
        ImGui::TableSetupColumn("Address", ImGuiTableColumnFlags_WidthFixed, smallColumn);
        ImGui::TableSetupColumn("Loaded", ImGuiTableColumnFlags_WidthFixed, smallColumn);
        ImGui::TableSetupColumn("Mode", ImGuiTableColumnFlags_PreferSortDescending | ImGuiTableColumnFlags_WidthStretch, smallColumn);
        ImGui::TableSetupScrollFreeze(0, 1); // Make row always visible
        ImGui::TableHeadersRow();

        std::vector<ModuleStatusBroadcast>& map = AppManager::getCanBusStream().getModuleStatuses();

        auto sortSpec = ImGui::TableGetSortSpecs();
        bool decending = sortSpec->Specs->SortDirection == ImGuiSortDirection_Descending;
        switch (sortSpec->Specs->ColumnIndex) {
        case 0: // name
            std::sort(map.begin(), map.end(), [decending](ModuleStatusBroadcast a, ModuleStatusBroadcast b) {
                if (decending)
                    return a.moduleAddress > b.moduleAddress;
                else
                    return a.moduleAddress < b.moduleAddress;
            });
            break;
        case 1: // address
            std::sort(map.begin(), map.end(), [decending](ModuleStatusBroadcast a, ModuleStatusBroadcast b) {
                if (decending)
                    return a.moduleAddress > b.moduleAddress;
                else
                    return a.moduleAddress < b.moduleAddress;
            });
            break;
        case 2: // loaded
            std::sort(map.begin(), map.end(), [decending](ModuleStatusBroadcast a, ModuleStatusBroadcast b) {
                if (decending)
                    return a.deserializeOk > b.deserializeOk;
                else
                    return a.deserializeOk < b.deserializeOk;
            });
            break;
        case 3: // mode
            std::sort(map.begin(), map.end(), [decending](ModuleStatusBroadcast a, ModuleStatusBroadcast b) {
                if (decending)
                    return a.mode > b.mode;
                else
                    return a.mode < b.mode;
            });
            break;
        }

        // All modules
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        std::string label = "All Modules";
        if (ImGui::Selectable(label.c_str(), m_selectedModuleAddr == CanCommands::AllModules, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowItemOverlap))
            m_selectedModuleAddr = CanCommands::AllModules;

        for (auto& ms : AppManager::getCanBusStream().getModuleStatuses()) {
            std::string modeString;
            switch (ms.mode) {
            case EBrytecApp::Mode::Normal:
                modeString = "Normal";
                break;
            case EBrytecApp::Mode::Stopped:
                modeString = "Stopped";
                break;
            }
            auto module = AppManager::getConfig()->findModule(ms.moduleAddress);

            ImGui::PushID(ms.moduleAddress);
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            std::string label;
            if (module)
                label = module->getName();
            else
                label = "Module: " + std::to_string(ms.moduleAddress);

            if (ImGui::Selectable(label.c_str(), m_selectedModuleAddr == ms.moduleAddress, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowItemOverlap))
                m_selectedModuleAddr = ms.moduleAddress;

            ImGui::TableNextColumn();
            ImGui::Text("%d", ms.moduleAddress);
            ImGui::TableNextColumn();
            ImGui::Text("%s", ms.deserializeOk ? "Yes" : "No");
            ImGui::TableNextColumn();
            ImGui::Text("%s", modeString.c_str());
            ImGui::PopID();
        }

        ImGui::EndTable();
    }
}

void CommunicationModals::drawModuleCommands()
{
    if (ImGui::Button("Set Stopped")) {
        AppManager::getCanBusStream().changeMode(m_selectedModuleAddr, EBrytecApp::Mode::Stopped);
        AppManager::getCanBusStream().send(std::bind(&CommunicationModals::callback, this, std::placeholders::_1));
    }

    ImGui::SameLine();

    if (ImGui::Button("Set Normal")) {
        AppManager::getCanBusStream().changeMode(m_selectedModuleAddr, EBrytecApp::Mode::Normal);
        AppManager::getCanBusStream().send(std::bind(&CommunicationModals::callback, this, std::placeholders::_1));
    }

    ImGui::SameLine();

    if (ImGui::Button("Reload")) {
        AppManager::getCanBusStream().reloadConfig(m_selectedModuleAddr);
        AppManager::getCanBusStream().send(std::bind(&CommunicationModals::callback, this, std::placeholders::_1));
    }

    ImGui::SameLine();

    // Check if module is stopped
    bool isModuleStopped = false;
    for (auto& ms : AppManager::getCanBusStream().getModuleStatuses()) {
        if (ms.moduleAddress == m_selectedModuleAddr)
            isModuleStopped = (ms.mode == EBrytecApp::Mode::Stopped);
    }

    bool addressDisabled = (m_selectedModuleAddr == CanCommands::AllModules) || !isModuleStopped;
    ImGui::BeginDisabled(addressDisabled);

    if (ImGui::Button("Change Address"))
        ImGui::OpenPopup("Address");

    ImGui::EndDisabled();

    if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled) && addressDisabled) {
        if (m_selectedModuleAddr == CanCommands::AllModules)
            ImGui::SetTooltip("%s", "Doesn't work for all modules");
        else
            ImGui::SetTooltip("%s", "Module must be stopped");
    }

    // Always center this window when appearing
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    if (ImGui::BeginPopupModal("Address", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {

        ImGui::Text("Select new address");
        ImGui::Text("Note: You must send a new config to make address permanent");

        static bool showButtons = true;
        ImGui::InputScalar("###ModuleAddressInput", ImGuiDataType_U8, &m_newAddress, &showButtons);

        auto module = AppManager::getConfig()->findModule(m_selectedModuleAddr);
        if (module) {
            std::string checkboxString = "Change - " + module->getName();
            ImGui::Checkbox(checkboxString.c_str(), &m_changeModuleAddress);
        }

        if (ImGui::Button("Cancel", { 120, 0 }))
            ImGui::CloseCurrentPopup();

        ImGui::SameLine();

        if (ImGui::Button("Ok", { 120, 0 })) {
            AppManager::getCanBusStream().changeAddress(m_selectedModuleAddr, m_newAddress);
            AppManager::getCanBusStream().send(std::bind(&CommunicationModals::callback, this, std::placeholders::_1));

            if (module && m_changeModuleAddress)
                module->setAddress(m_newAddress);

            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

void CommunicationModals::drawSendReceive()
{
    auto module = AppManager::getConfig()->findModule(m_selectedModuleAddr);

    bool templateDisabled = (bool)module || m_selectedModuleAddr == CanCommands::AllModules;
    ImGui::BeginDisabled(templateDisabled);

    if (ImGui::Button("Get Module Template")) {
        AppManager::getCanBusStream().getModuleData(
            m_selectedModuleAddr,
            [=](const std::vector<uint8_t>& buffer) {
                std::shared_ptr<Module> module = std::make_shared<Module>();
                ModuleSerializer serializer(module);
                BinaryArrayDeserializer des(buffer.data(), buffer.size());
                if (serializer.deserializeTemplateBinary(des)) {
                    module->setAddress(m_selectedModuleAddr);
                    AppManager::getConfig()->addModule(module);
                }
            },
            false);
        AppManager::getCanBusStream().send(std::bind(&CommunicationModals::callback, this, std::placeholders::_1));
    }

    ImGui::EndDisabled();

    if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled) && templateDisabled) {
        if (m_selectedModuleAddr == CanCommands::AllModules)
            ImGui::SetTooltip("%s", "Doesn't work for all modules");
        else
            ImGui::SetTooltip("%s", "There is already a module for selected");
    }

    ImGui::SameLine();

    bool configDisabled = !module || m_selectedModuleAddr == CanCommands::AllModules;
    ImGui::BeginDisabled(configDisabled);

    if (ImGui::Button("Get From Module")) {
        AppManager::getCanBusStream().getModuleData(
            m_selectedModuleAddr,
            [=](const std::vector<uint8_t>& buffer) {
                std::shared_ptr<Module> module = AppManager::getConfig()->findModule(m_selectedModuleAddr);
                if (module) {

                    // Remove all old node groups to get ready for new ones from reading module
                    for (auto& phy : module->getPhysicalPins())
                        phy->setNodeGroup(nullptr);

                    for (auto& internal : module->getInternalPins())
                        internal->setNodeGroup(nullptr);
                    module->updateInternalPins();

                    ModuleSerializer serializer(AppManager::getConfig(), module);
                    BinaryArrayDeserializer des(buffer.data(), buffer.size());
                    serializer.deserializeBinary(des);
                } else {
                    std::cout << "Need to get module template first" << std::endl;
                }
            },
            true);
        AppManager::getCanBusStream().send(std::bind(&CommunicationModals::callback, this, std::placeholders::_1));
    }

    ImGui::EndDisabled();

    if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled) && configDisabled) {
        if (m_selectedModuleAddr == CanCommands::AllModules)
            ImGui::SetTooltip("%s", "Doesn't work for all modules");
        else
            ImGui::SetTooltip("%s", "You must have a module for selected");
    }

    ImGui::SameLine();

    ImGui::BeginDisabled(configDisabled);

    if (ImGui::Button("Send To Module")) {
        if (module) {
            ModuleSerializer moduleSer(module);
            BinarySerializer ser = moduleSer.serializeBinary();
            AppManager::getCanBusStream().sendNewConfig(module->getAddress(), ser.getData());
            AppManager::getCanBusStream().send(std::bind(&CommunicationModals::callback, this, std::placeholders::_1));
        }
    }

    ImGui::EndDisabled();

    if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled) && configDisabled) {
        if (m_selectedModuleAddr == CanCommands::AllModules)
            ImGui::SetTooltip("%s", "Doesn't work for all modules");
        else
            ImGui::SetTooltip("%s", "You must have a module for selected");
    }
}

void CommunicationModals::drawProgressBar()
{
    if (!m_callbackData.error)
        ImGui::ProgressBar((float)(m_callbackData.total - m_callbackData.leftToSend) / (float)m_callbackData.total);
    else
        ImGui::TextUnformatted("Error Sending Data");
}

void CommunicationModals::drawCloseButton()
{
    ImGui::Separator();

    if (ImGui::Button("Close", ImVec2(120, 0)))
        ImGui::CloseCurrentPopup();
}

void CommunicationModals::callback(CanBusStreamCallbackData data)
{
    m_callbackData = data;

    if (m_callbackData.leftToSend == 0) {
        AppManager::getCanBusStream().getModuleStatuses().clear();
        AppManager::getCanBusStream().requestModuleStatus(CanCommands::AllModules);
        AppManager::getCanBusStream().send([](CanBusStreamCallbackData data) {});
    }
}
}