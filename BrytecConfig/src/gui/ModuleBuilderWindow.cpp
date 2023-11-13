#include "ModuleBuilderWindow.h"

#include "AppManager.h"
#include "data/InternalPin.h"
#include "data/PhysicalPin.h"
#include "utils/FileDialogs.h"
#include "utils/ModuleSerializer.h"
#include <IconsFontAwesome5.h>
#include <fstream>
#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

namespace Brytec {

ModuleBuilderWindow::ModuleBuilderWindow()
{
    m_module = std::make_shared<Module>();
}

void ModuleBuilderWindow::drawWindow()
{
    if (!m_opened)
        return;

    ImGui::Begin(ICON_FA_DICE_D6 " Module Builder", &m_opened, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking);

    drawMenubar();
    drawModuleTable();

    ImGui::End();
}

void ModuleBuilderWindow::drawMenubar()
{
    if (ImGui::BeginMenuBar()) {

        // New
        if (ImGui::MenuItem(ICON_FA_FILE))
            m_module = std::make_shared<Module>();

        // Open
        if (ImGui::MenuItem(ICON_FA_FOLDER_OPEN)) {
            auto modulePath = FileDialogs::OpenFile("btmodule", AppManager::getModulesPath());
            if (!modulePath.empty()) {
                std::shared_ptr<Module> module = std::make_shared<Module>();
                ModuleSerializer serializer(module);
                BinaryPathDeserializer des(modulePath);
                if (serializer.deserializeTemplateBinary(des)) {
                    m_module = module;
                } else {
                    std::cout << "Could not deserialize module template file" << std::endl;
                }
            }
        }

        // Save
        if (ImGui::MenuItem(ICON_FA_SAVE)) {
            auto path = FileDialogs::SaveFile("btmodule", AppManager::getModulesPath());

            if (path.extension().empty())
                path.replace_extension("btmodule");

            if (!path.empty()) {
                ModuleSerializer serializer(m_module);
                auto moduleTemplateBinary = serializer.serializeTemplateBinary();
                moduleTemplateBinary.writeToFile(path);
            }
        }

        // Add Pin
        if (ImGui::MenuItem(ICON_FA_PLUS_CIRCLE))
            m_editPin = m_module->addPhysicalPin();

        // Pin assignment defines
        if (ImGui::MenuItem("Save Defines")) {
            auto path = FileDialogs::SaveFile("h");

            if (path.extension().empty())
                path.replace_extension("h");

            if (!path.empty())
                writeDefineFile(path);
        }

        ImGui::EndMenuBar();
    }
}

void ModuleBuilderWindow::drawModuleTable()
{
    static ImGuiTableFlags flags = ImGuiTableFlags_PadOuterX | ImGuiTableFlags_Resizable | ImGuiTableFlags_RowBg | ImGuiTableFlags_NoBordersInBody | ImGuiTableFlags_NoSavedSettings;

    if (ImGui::BeginTable("ModuleTable", 2, flags)) {
        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed, 150.0f);
        ImGui::TableSetupColumn("Value");

        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::AlignTextToFramePadding();
        ImGui::Text("Name");
        ImGui::TableNextColumn();
        ImGui::SetNextItemWidth(-FLT_MIN);
        ImGui::InputText("###MouduleName", &m_module->getName(), ImGuiInputTextFlags_AutoSelectAll);

        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::AlignTextToFramePadding();
        ImGui::Text("Manufacturer");
        ImGui::TableNextColumn();
        ImGui::SetNextItemWidth(-FLT_MIN);
        ImGui::InputText("###ManufacturerName", &m_module->getManufacturerName(), ImGuiInputTextFlags_AutoSelectAll);

        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::AlignTextToFramePadding();
        ImGui::Text("Board");
        ImGui::TableNextColumn();
        ImGui::SetNextItemWidth(-FLT_MIN);
        ImGui::InputText("###BoardName", &m_module->getBoardName(), ImGuiInputTextFlags_AutoSelectAll);

        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::AlignTextToFramePadding();
        ImGui::Text("Default Address");
        ImGui::TableNextColumn();
        static bool showButtons = true;
        ImGui::SetNextItemWidth(-FLT_MIN);
        ImGui::InputScalar("###ModuleAddressInput", ImGuiDataType_U8, &m_module->getAddress(), &showButtons);

        ImGui::EndTable();

        const int columns_count = (int)IOTypes::Types::Count + 2;
        static ImGuiTableFlags table_flags = ImGuiTableFlags_PadOuterX | ImGuiTableFlags_Resizable | ImGuiTableFlags_ScrollX | ImGuiTableFlags_ScrollY | ImGuiTableFlags_HighlightHoveredColumn | ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_NoSavedSettings;
        float columnWidth = ImGui::GetFrameHeight();

        if (ImGui::BeginTable("Pins", columns_count, table_flags)) {
            ImGui::TableSetupColumn("Pins", ImGuiTableColumnFlags_WidthFixed, 150.0f);

            ImGui::TableSetupColumn("Max Current (Amps)", ImGuiTableColumnFlags_AngledHeader | ImGuiTableColumnFlags_WidthFixed, 100.0f);
            ImGui::TableSetupColumn("Pwm", ImGuiTableColumnFlags_AngledHeader | ImGuiTableColumnFlags_WidthFixed, columnWidth);
            for (int n = 1; n < (int)IOTypes::Types::Count; n++)
                ImGui::TableSetupColumn(IOTypes::Strings[n], ImGuiTableColumnFlags_AngledHeader | ImGuiTableColumnFlags_WidthFixed, columnWidth);

            ImGui::TableSetupScrollFreeze(1, 2);

            ImGui::TableAngledHeadersRow();
            ImGui::TableHeadersRow();

            for (int i = 0; i < m_module->getPhysicalPins().size(); i++) {

                auto& pin = m_module->getPhysicalPins()[i];
                ImGui::PushID(i);

                // Pinout Name
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::AlignTextToFramePadding();
                ImGui::SetNextItemWidth(-FLT_MIN);
                ImGui::InputText("###PinoutNameInput", &pin->getPinoutName(), ImGuiInputTextFlags_AutoSelectAll);

                // Max Current
                ImGui::TableNextColumn();
                static bool showButtons = true;
                ImGui::SetNextItemWidth(-FLT_MIN);
                uint8_t maxCurrent = pin->getMaxCurrent();
                if (ImGui::InputScalar("###ModuleAddressInput", ImGuiDataType_U8, &maxCurrent, &showButtons))
                    pin->setMaxCurrent(maxCurrent);

                // Pwm
                ImGui::TableNextColumn();
                bool pwm = pin->getPwm();
                if (ImGui::Checkbox("###Pwm", &pwm))
                    pin->setPwm(pwm);

                // Start at 1 to ignore undefined type
                for (int j = 1; j < (int)IOTypes::Types::Count; j++) {
                    ImGui::PushID(j);
                    ImGui::TableNextColumn();

                    bool checkState = std::find(pin->getAvailableTypes().begin(), pin->getAvailableTypes().end(), (IOTypes::Types)j) != pin->getAvailableTypes().end();
                    if (ImGui::Checkbox("###checkbox", &checkState)) {
                        bool containsType = std::find(pin->getAvailableTypes().begin(), pin->getAvailableTypes().end(), (IOTypes::Types)j) != pin->getAvailableTypes().end();
                        if (!containsType)
                            pin->getAvailableTypes().push_back((IOTypes::Types)j);
                        else
                            pin->getAvailableTypes().erase(std::find(pin->getAvailableTypes().begin(), pin->getAvailableTypes().end(), (IOTypes::Types)j));
                    }
                    ImGui::PopID();
                }
                ImGui::PopID();
            }
            ImGui::EndTable();
        }
    }
}

void ModuleBuilderWindow::writeDefineFile(const std::filesystem::path& path)
{
    std::ofstream fout(path);
    fout << "#pragma once"
         << std::endl
         << std::endl;

    fout << "#include <stdint.h>"
         << std::endl
         << std::endl;

    {
        fout << "// Pin Defines" << std::endl;
        for (int i = 0; i < m_module->getPhysicalPins().size(); i++) {
            auto& pin = m_module->getPhysicalPins()[i];
            std::string pinout = pin->getPinoutName();
            std::replace(pinout.begin(), pinout.end(), ' ', '_');
            fout << "#define BT_PIN_" << pinout << " " << i << "\n";
        }
    }

    fout << std::endl;

    {
        ModuleSerializer moduleSer(m_module);
        BinarySerializer ser = moduleSer.serializeTemplateBinary();

        fout << "// Module Template" << std::endl;
        fout << "constexpr uint8_t moduleTemplate[] = {" << std::endl;
        for (auto d : ser.getData()) {
            fout << std::showbase << std::hex << (int)d << ",";
        }
        fout << std::endl
             << "};" << std::endl;
    }
}
}