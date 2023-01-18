#include "ModuleBuilderWindow.h"

#include "AppManager.h"
#include "utils/DefaultPaths.h"
#include "utils/FileDialogs.h"
#include "utils/ModuleSerializer.h"
#include <IconsFontAwesome5.h>
#include <fstream>
#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

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
            auto modulePath = FileDialogs::OpenFile("btmodule", MODULES_PATH);
            if (!modulePath.empty()) {
                std::shared_ptr<Module> module = std::make_shared<Module>();
                ModuleSerializer serializer(module);
                BinaryDeserializer des;
                des.setDataFromPath(modulePath);
                if (serializer.deserializeTemplateBinary(des)) {
                    m_module = module;
                } else {
                    std::cout << "Could not deserialize module template file" << std::endl;
                }
            }
        }

        // Save
        if (ImGui::MenuItem(ICON_FA_SAVE)) {
            auto path = FileDialogs::SaveFile("btmodule", MODULES_PATH);

            if (path.extension().empty())
                path.replace_extension("btmodule");

            if (!path.empty()) {
                ModuleSerializer serializer(m_module);
                auto moduleTemplateBinary = serializer.serializeTemplateBinary();
                moduleTemplateBinary.writeToFile(path);
            }
        }

        // Add Pin
        if (ImGui::MenuItem(ICON_FA_PLUS_CIRCLE)) {
            m_editPin = m_module->addPhysicalPin();
            ImGui::OpenPopup("Pin Dialog");
        }
        drawPinDialog(m_editPin);

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
    static ImGuiTableFlags flags = ImGuiTableFlags_BordersV | ImGuiTableFlags_BordersOuterH | ImGuiTableFlags_Resizable | ImGuiTableFlags_RowBg | ImGuiTableFlags_NoBordersInBody;

    static ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_FramePadding;
    static ImGuiTreeNodeFlags leafNodeFlags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_Bullet | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_FramePadding;

    if (ImGui::BeginTable("3ways", 2, flags)) {
        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_NoHide | ImGuiTableColumnFlags_WidthFixed, 200.0f);
        ImGui::TableSetupColumn("Value");
        ImGui::TableHeadersRow();

        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        if (ImGui::TreeNodeEx("Module", nodeFlags)) {
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::TreeNodeEx("Name", leafNodeFlags);
            ImGui::TableNextColumn();
            ImGui::SetNextItemWidth(-FLT_MIN);
            ImGui::InputText("###MouduleName", &m_module->getName());

            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::TreeNodeEx("Default Address", leafNodeFlags);
            ImGui::TableNextColumn();
            static bool showButtons = true;
            ImGui::SetNextItemWidth(-FLT_MIN);
            ImGui::InputScalar("###ModuleAddressInput", ImGuiDataType_U8, &m_module->getAddress(), &showButtons);

            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            if (ImGui::TreeNodeEx("Pins", nodeFlags)) {

                for (int i = 0; i < m_module->getPhysicalPins().size(); i++) {
                    auto& pin = m_module->getPhysicalPins()[i];

                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::PushID(i);

                    if (ImGui::TreeNodeEx("###PinoutName", ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_FramePadding, "%s%d%s %s", "(", i, ")", pin->getPinoutName().c_str())) {

                        // Name
                        ImGui::TableNextColumn();
                        ImGui::SetNextItemWidth(-FLT_MIN);
                        ImGui::InputText("###PinoutNameInput", &pin->getPinoutName());

                        // Current
                        ImGui::TableNextRow();
                        ImGui::TableNextColumn();
                        ImGui::TreeNodeEx("Max Current", leafNodeFlags);
                        ImGui::TableNextColumn();
                        static bool showButtons = true;
                        ImGui::SetNextItemWidth(-FLT_MIN);
                        uint8_t maxCurrent = pin->getMaxCurrent();
                        if (ImGui::InputScalar("###ModuleAddressInput", ImGuiDataType_U8, &maxCurrent, &showButtons))
                            pin->setMaxCurrent(maxCurrent);

                        // Pwm
                        ImGui::TableNextRow();
                        ImGui::TableNextColumn();
                        ImGui::TreeNodeEx("Pwm", leafNodeFlags);
                        ImGui::TableNextColumn();
                        ImGui::SetNextItemWidth(-FLT_MIN);
                        bool pwm = pin->getPwm();
                        if (ImGui::Checkbox("###Pwm", &pwm))
                            pin->setPwm(pwm);

                        // Start at 1 to ignore undefined type
                        for (int j = 1; j < (int)IOTypes::Types::Count; j++) {
                            ImGui::PushID(j);
                            ImGui::TableNextRow();
                            ImGui::TableNextColumn();
                            ImGui::TreeNodeEx(IOTypes::Strings[j], leafNodeFlags);
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

                        ImGui::TableNextRow();
                        ImGui::TableNextColumn();
                        ImGui::TableNextColumn();
                        if (ImGui::Button("Delete")) {
                            m_module->deletePin(pin);
                            ImGui::PopID();
                            ImGui::TreePop();
                            ImGui::TreePop();
                            ImGui::TreePop();
                            ImGui::EndTable();
                            return;
                        }

                        ImGui::TreePop();
                    } else {
                        // Up and down arrows
                        ImGui::TableNextColumn();
                        if (ImGui::Button(ICON_FA_ARROW_UP)) {
                            if (i != 0)
                                std::swap(pin, m_module->getPhysicalPins()[i - 1]);
                        }
                        ImGui::SameLine();
                        if (ImGui::Button(ICON_FA_ARROW_DOWN)) {
                            if (i != m_module->getPhysicalPins().size() - 1)
                                std::swap(pin, m_module->getPhysicalPins()[i + 1]);
                        }
                        ImGui::SameLine();
                        if (ImGui::Button("Edit")) {
                            m_editPin = pin;
                            ImGui::OpenPopup("Pin Dialog");
                        }
                        drawPinDialog(m_editPin);
                    }
                    ImGui::PopID();
                }

                ImGui::TreePop();
            }

            ImGui::TreePop();
        }

        ImGui::EndTable();
    }
}

void ModuleBuilderWindow::drawPinDialog(std::shared_ptr<PhysicalPin>& pin)
{

    static ImGuiTableFlags flags = ImGuiTableFlags_BordersV | ImGuiTableFlags_BordersOuterH | ImGuiTableFlags_Resizable | ImGuiTableFlags_RowBg | ImGuiTableFlags_NoBordersInBody;
    static ImGuiTreeNodeFlags leafNodeFlags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_Bullet | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_FramePadding;
    bool open = true;
    if (ImGui::BeginPopupModal("Pin Dialog", &open, ImGuiWindowFlags_AlwaysAutoResize)) {
        if (ImGui::BeginTable("3ways", 2, flags, ImVec2(500.0f, 0.0f))) {
            ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_NoHide | ImGuiTableColumnFlags_WidthFixed, 200.0f);
            ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableHeadersRow();

            // Name
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::TreeNodeEx("Pinout", leafNodeFlags);
            ImGui::TableNextColumn();
            ImGui::SetNextItemWidth(-FLT_MIN);
            ImGui::InputText("###PinoutNameInput", &pin->getPinoutName(), ImGuiInputTextFlags_AutoSelectAll);

            // Current
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::TreeNodeEx("Max Current", leafNodeFlags);
            ImGui::TableNextColumn();
            static bool showButtons = true;
            ImGui::SetNextItemWidth(-FLT_MIN);
            uint8_t maxCurrent = pin->getMaxCurrent();
            if (ImGui::InputScalar("###ModuleAddressInput", ImGuiDataType_U8, &maxCurrent, &showButtons))
                pin->setMaxCurrent(maxCurrent);

            // Pwm
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::TreeNodeEx("Pwm", leafNodeFlags);
            ImGui::TableNextColumn();
            ImGui::SetNextItemWidth(-FLT_MIN);
            bool pwm = pin->getPwm();
            if (ImGui::Checkbox("###Pwm", &pwm))
                pin->setPwm(pwm);

            // Start at 1 to ignore undefined type
            for (int j = 1; j < (int)IOTypes::Types::Count; j++) {
                ImGui::PushID(j);
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::TreeNodeEx(IOTypes::Strings[j], leafNodeFlags);
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

            ImGui::EndTable();

            ImGui::EndPopup();
        }
    }
}

void ModuleBuilderWindow::writeDefineFile(const std::filesystem::path& path)
{
    std::ofstream fout(path, std::ios::trunc);
    fout << "#pragma once\n\n";

    for (int i = 0; i < m_module->getPhysicalPins().size(); i++) {
        auto& pin = m_module->getPhysicalPins()[i];
        std::string pinout = pin->getPinoutName();
        std::replace(pinout.begin(), pinout.end(), ' ', '_');
        fout << "#define BT_PIN_" << pinout << " " << i << "\n";
    }
}