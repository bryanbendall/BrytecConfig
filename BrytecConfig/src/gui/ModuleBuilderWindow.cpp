#include "ModuleBuilderWindow.h"

#include <IconsFontAwesome5.h>
#include <imgui.h>
#include "../AppManager.h"
#include <misc/cpp/imgui_stdlib.h>
#include "../utils/ModuleBuilder.h"

ModuleBuilderWindow::ModuleBuilderWindow()
{
    m_module = std::make_shared<Module>();
}

void ModuleBuilderWindow::drawWindow()
{
    if(!m_opened)
        return;

    ImGui::Begin(ICON_FA_DICE_D6" Module Builder", &m_opened, ImGuiWindowFlags_MenuBar);

    drawMenubar();
    drawModuleTable();

    ImGui::End();
}

void ModuleBuilderWindow::drawMenubar()
{
    if(ImGui::BeginMenuBar()) {

        if(ImGui::MenuItem("New Module"))
            m_module = std::make_shared<Module>();

        if(ImGui::MenuItem("Save Module")) {
            std::string filepath = "data/modules/";
            filepath += m_module->getName();
            filepath += ".yaml";
            ModuleBuilder::saveModuleFile(std::filesystem::path(filepath), m_module);
        }

        if(ImGui::MenuItem("Add Pin"))
            m_module->addPin();

        ImGui::EndMenuBar();
    }
}

void ModuleBuilderWindow::drawModuleTable()
{
    static ImGuiTableFlags flags = ImGuiTableFlags_BordersV | ImGuiTableFlags_BordersOuterH | ImGuiTableFlags_Resizable | ImGuiTableFlags_RowBg | ImGuiTableFlags_NoBordersInBody;

    static ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_FramePadding;
    static ImGuiTreeNodeFlags leafNodeFlags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_Bullet | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_FramePadding;

    if(ImGui::BeginTable("3ways", 2, flags)) {
        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_NoHide);
        ImGui::TableSetupColumn("Value");
        ImGui::TableHeadersRow();

        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        if(ImGui::TreeNodeEx("Module", nodeFlags)) {
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
            if(ImGui::TreeNodeEx("Pins", nodeFlags)) {

                for(auto& pin : m_module->getPins()) {

                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::PushID(&pin);

                    if(ImGui::TreeNodeEx("###PinoutName", ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_FramePadding, pin->getPinoutName().c_str())) {

                        ImGui::TableNextColumn();
                        ImGui::SetNextItemWidth(-FLT_MIN);
                        ImGui::InputText("###PinoutNameInput", &pin->getPinoutName());

                        // Start at 1 to ignore undefined type
                        for(int j = 1; j < (int) IOTypes::Types::Count; j++) {
                            ImGui::PushID(j);
                            ImGui::TableNextRow();
                            ImGui::TableNextColumn();
                            ImGui::TreeNodeEx(IOTypes::Strings[j], leafNodeFlags);
                            ImGui::TableNextColumn();

                            bool containsType = std::find(pin->getAvailableTypes().begin(), pin->getAvailableTypes().end(), (IOTypes::Types) j) != pin->getAvailableTypes().end();
                            if(ImGui::Checkbox("###checkbox", &containsType)) {
                                pin->getAvailableTypes().push_back((IOTypes::Types) j);
                            }
                            ImGui::PopID();
                        }

                        ImGui::TableNextRow();
                        ImGui::TableNextColumn();
                        ImGui::TableNextColumn();
                        if(ImGui::Button("Delete")) {
                            m_module->deletePin(pin);
                            ImGui::PopID();
                            ImGui::TreePop();
                            ImGui::TreePop();
                            ImGui::TreePop();
                            ImGui::EndTable();
                            return;
                        }

                        ImGui::TreePop();
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
