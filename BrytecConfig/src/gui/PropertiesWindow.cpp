#include "PropertiesWindow.h"

#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>
#include "../AppManager.h"
#include <IconsFontAwesome5.h>
#include <memory>

static ImGuiTableFlags flags = ImGuiTableFlags_BordersV | ImGuiTableFlags_BordersOuterH | ImGuiTableFlags_Resizable | ImGuiTableFlags_RowBg | ImGuiTableFlags_NoBordersInBody;

void PropertiesWindow::drawWindow() {
	if (!m_opened)
		return;

	ImGui::Begin(ICON_FA_COG" Properties", &m_opened);

	std::shared_ptr<Selectable> selected = AppManager::getSelectedItem().lock();
	if (selected) {
		
		if (auto module = std::dynamic_pointer_cast<Module>(selected))
			drawModuleProps(module);

		if(auto pin = std::dynamic_pointer_cast<Pin>(selected)) {
			drawPinProps(pin);

			if(auto nodeGroup = pin->getNodeGroup())
				drawNodeGroupProps(nodeGroup);
		}
		
		if(auto nodeGroup = std::dynamic_pointer_cast<NodeGroup>(selected))
			drawNodeGroupProps(nodeGroup);
		
	}

	ImGui::End();
}

void PropertiesWindow::drawModuleProps(std::shared_ptr<Module> module)
{

	if(ImGui::BeginTable("ModuleProps", 2, flags)) {

		ImGui::TableSetupColumn("Module", ImGuiTableColumnFlags_NoHide | ImGuiTableColumnFlags_WidthFixed, 100.0f);
		ImGui::TableHeadersRow();

		// Name
		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		ImGui::AlignTextToFramePadding();
		ImGui::Text("Name");
		ImGui::TableNextColumn();
		ImGui::SetNextItemWidth(-FLT_MIN);
		ImGui::InputText("###MouduleName", &module->getName());

		// Address
		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		ImGui::AlignTextToFramePadding();
		ImGui::Text("Address");
		ImGui::TableNextColumn();
		static bool showButtons = true;
		ImGui::SetNextItemWidth(-FLT_MIN);
		ImGui::InputScalar("###ModuleAddressInput", ImGuiDataType_U8, &module->getAddress(), &showButtons);

		// Enabled
		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		ImGui::AlignTextToFramePadding();
		ImGui::Text("Enabled");
		ImGui::TableNextColumn();
		ImGui::SetNextItemWidth(-FLT_MIN);
		ImGui::Checkbox("###MouduleEnabled", &module->getEnabled());

		// Total Nodes
		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		ImGui::AlignTextToFramePadding();
		ImGui::Text("Total Nodes");
		ImGui::TableNextColumn();
		ImGui::SetNextItemWidth(-FLT_MIN);
		int totalNodes = 0;
		for(auto& pin : module->getPins()) {
			if(auto& ng = pin->getNodeGroup())
				totalNodes += ng->getNodes().size();
		}
		ImGui::Text("%d", totalNodes);

		ImGui::EndTable();
	}

}

void PropertiesWindow::drawPinProps(std::shared_ptr<Pin> pin)
{

	if(ImGui::BeginTable("PinProps", 2, flags)) {

		ImGui::TableSetupColumn("Pin", ImGuiTableColumnFlags_NoHide | ImGuiTableColumnFlags_WidthFixed, 100.0f);
		ImGui::TableHeadersRow();

		// Name
		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		ImGui::AlignTextToFramePadding();
		ImGui::Text("Pinout Name");
		ImGui::TableNextColumn();
		ImGui::SetNextItemWidth(-FLT_MIN);
		ImGui::Text(pin->getPinoutName().c_str());

		// Current Limit
		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		ImGui::AlignTextToFramePadding();
		ImGui::Text("Current Limit");
		ImGui::TableNextColumn();
		ImGui::SetNextItemWidth(-FLT_MIN);
		ImGui::Combo("###Current Limit", (int*) &pin->getCurrentLimit(), Pin::currentNames, IM_ARRAYSIZE(Pin::currentNames));

		// Node Group
		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		ImGui::AlignTextToFramePadding();
		ImGui::Text("Node Group");
		ImGui::TableNextColumn();
		ImGui::SetNextItemWidth(-FLT_MIN);
		if(pin->getNodeGroup())
			ImGui::Text(pin->getNodeGroup()->getName().c_str());
		else
			ImGui::TextDisabled("None");

		if(pin->getNodeGroup()) {
			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			ImGui::TableNextColumn();
			if(ImGui::Button("Remove Node Group"))
				pin->setNodeGroup(nullptr);
		}

		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		if(ImGui::TreeNodeEx("Types", ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_DefaultOpen)) {

			for(auto& type : pin->getAvailableTypes()) {
				ImGui::PushID((int)type);
				ImGui::TableNextRow();
				ImGui::TableNextColumn();
				ImGui::TableNextColumn();
				ImGui::AlignTextToFramePadding();
				ImGui::Text(IOTypes::getString(type));

				ImGui::PopID();
			}

			ImGui::TreePop();
		}

		ImGui::EndTable();
	}

}

void PropertiesWindow::drawNodeGroupProps(std::shared_ptr<NodeGroup> nodeGroup) 
{
	if(ImGui::BeginTable("NodeGroupProps", 2, flags)) {

		ImGui::TableSetupColumn("Node Group", ImGuiTableColumnFlags_NoHide | ImGuiTableColumnFlags_WidthFixed, 100.0f);
		ImGui::TableHeadersRow();

		// Name
		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		ImGui::AlignTextToFramePadding();
		ImGui::Text("Name");
		ImGui::TableNextColumn();
		ImGui::SetNextItemWidth(-FLT_MIN);
		ImGui::InputText("###pinName", &nodeGroup->getName());

		// Id
		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		ImGui::AlignTextToFramePadding();
		ImGui::Text("Id");
		ImGui::TableNextColumn();
		ImGui::SetNextItemWidth(-FLT_MIN);
		ImGui::Text("%llu", nodeGroup->getId());

		// Enabled
		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		ImGui::AlignTextToFramePadding();
		ImGui::Text("Enabled");
		ImGui::TableNextColumn();
		ImGui::SetNextItemWidth(-FLT_MIN);
		ImGui::Checkbox("###MouduleEnabled", &nodeGroup->getEnabled());

		// Type
		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		ImGui::AlignTextToFramePadding();
		ImGui::Text("Type");
		ImGui::TableNextColumn();
		ImGui::SetNextItemWidth(-FLT_MIN);
		if(nodeGroup->getAssigned()) {
			ImGui::Text("%s %s", IOTypes::getString(nodeGroup->getType()), ICON_FA_LOCK);
		} else {
			ImGui::Combo("###Node Group Type", (int*) &nodeGroup->getType() , IOTypes::Strings, IM_ARRAYSIZE(IOTypes::Strings), 10);
		}

		// Total Nodes
		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		ImGui::AlignTextToFramePadding();
		ImGui::Text("Total Nodes");
		ImGui::TableNextColumn();
		ImGui::SetNextItemWidth(-FLT_MIN);
		ImGui::Text("%d", nodeGroup->getNodes().size());

		ImGui::EndTable();
	}

	// Nodes
	ImGui::Separator();
	for(auto n : nodeGroup->getNodes()) {
		ImGui::LabelText(n->getName().c_str(), "%i", n->getId());
	}
}
