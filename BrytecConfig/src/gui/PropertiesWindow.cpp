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

			//auto nodeGroup = pin->getNodeGroup();
			//if(nodeGroup)
			//	drawNodeGroupProps(nodeGroup);
		}

		
		if(auto nodeGroup = std::dynamic_pointer_cast<NodeGroup>(selected))
			drawNodeGroupProps(nodeGroup);
		
	}

	ImGui::End();

	drawStatsWindow();
}

void PropertiesWindow::drawModuleProps(std::shared_ptr<Module> module)
{

	if(ImGui::BeginTable("ModuleProps", 2, flags)) {

		ImGui::TableSetupColumn("Module", ImGuiTableColumnFlags_NoHide);
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

		ImGui::EndTable();
	}

}

void PropertiesWindow::drawPinProps(std::shared_ptr<Pin> pin)
{

	if(ImGui::BeginTable("PinProps", 2, flags)) {

		ImGui::TableSetupColumn("Pin", ImGuiTableColumnFlags_NoHide);
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
		std::string nodeGroup = pin->getNodeGroup() ? pin->getNodeGroup()->getName() : "Not Assigned";
		if(ImGui::Selectable(nodeGroup.c_str())) {
			if(pin->getNodeGroup())
				AppManager::setSelected(pin->getNodeGroup());
		}

		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		if(ImGui::TreeNodeEx("Types", ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_FramePadding)) {

			// Start at 1 to ignore undefined type
			for(int j = 1; j < (int) IOTypes::Types::Count; j++) {
				ImGui::PushID(j);
				ImGui::TableNextRow();
				ImGui::TableNextColumn();
				ImGui::TableNextColumn();
				ImGui::AlignTextToFramePadding();
				ImGui::Text(IOTypes::Strings[j]);

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

		ImGui::TableSetupColumn("I/O", ImGuiTableColumnFlags_NoHide);
		ImGui::TableHeadersRow();

		// Name
		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		ImGui::AlignTextToFramePadding();
		ImGui::Text("Name");
		ImGui::TableNextColumn();
		ImGui::SetNextItemWidth(-FLT_MIN);
		ImGui::InputText("###pinName", &nodeGroup->getName());

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
		ImGui::Combo("###Node Group Type", (int*) &nodeGroup->getType() , IOTypes::Strings, IM_ARRAYSIZE(IOTypes::Strings), 10);

		ImGui::EndTable();
	}

	// Nodes
	ImGui::Separator();
	for(auto n : nodeGroup->getNodes()) {
		ImGui::LabelText(n->getName().c_str(), "%i", n->getId());
	}
}

void PropertiesWindow::drawStats(std::shared_ptr<Module> module) 
{
	// Nodes in module
	ImGui::TextUnformatted("Module Nodes");
	ImGui::NextColumn();
	ImGui::SetNextItemWidth(-FLT_MIN);
	int totalNodes = 0;
	for(auto pin : module->getPins())
		totalNodes += pin->getNodeGroup() ? pin->getNodeGroup()->getNodes().size() :0;
	ImGui::Text("%i", totalNodes);
	ImGui::NextColumn();

	// Bytes in module
	int totalBytes = 0;
	for(auto p : module->getPins()) {
		if(p->getNodeGroup()) {
			for(auto n : p->getNodeGroup()->getNodes())
				totalBytes += n->getSize();
		}
	}
	ImGui::TextUnformatted("Bytes in module");
	ImGui::NextColumn();
	ImGui::Text("%i %s", totalBytes, "bytes");
	ImGui::NextColumn();

	// Percent memory used in module
	ImGui::TextUnformatted("Memory used");
	ImGui::NextColumn();
	ImGui::Text("%.2f%s", ((float) totalBytes / 15000.0f * 100.0f), "% used");
	ImGui::NextColumn();
}

void PropertiesWindow::drawStats(std::shared_ptr<Pin> pin)
{
	// Node in pin
	ImGui::TextUnformatted("Pin Nodes");
	ImGui::NextColumn();
	ImGui::SetNextItemWidth(-FLT_MIN);
	ImGui::Text("%i", pin->getNodeGroup()->getNodes().size());
	ImGui::NextColumn();

	// Bytes in pin
	int totalBytes = 0;
	for(auto n : pin->getNodeGroup()->getNodes())
		totalBytes += n->getSize();
	ImGui::TextUnformatted("Bytes in pin");
	ImGui::NextColumn();
	ImGui::Text("%i %s", totalBytes, "bytes");
	ImGui::NextColumn();

	// Percent Memory used in pin
	ImGui::TextUnformatted("Memory used");
	ImGui::NextColumn();
	ImGui::Text("%.2f%s", ((float)totalBytes / 15000.0f * 100.0f), "% used");
	ImGui::NextColumn();
}

void PropertiesWindow::drawStatsWindow() 
{
	// Title
	ImGui::Begin(ICON_FA_INFO_CIRCLE" Stats");
	ImGui::TextUnformatted(ICON_FA_INFO_CIRCLE" Total Stats");

	// Columns setup
	ImGui::Separator();
	ImGui::Columns(2, "###Column", true);
	ImGui::SetColumnWidth(-1, 100.0f);

	// FPS
	float frameTime = ImGui::GetIO().DeltaTime * 1000.0f;
	ImGui::TextUnformatted("Frame Time");
	ImGui::NextColumn();
	ImGui::Text("%4.2f", frameTime);
	ImGui::NextColumn();
	float fps = 1.0f / ImGui::GetIO().DeltaTime;
	ImGui::TextUnformatted("Fps");
	ImGui::NextColumn();
	ImGui::Text("%4.1f", fps);
	ImGui::NextColumn();

	// Numer of modules
	ImGui::TextUnformatted("Total Modules");
	ImGui::NextColumn();
	ImGui::Text("%i", AppManager::getConfig().getModules().size());
	ImGui::NextColumn();

	/*
	// Calculating number of objects
	int totalPins = 0;
	int totalNodes = 0;
	int totalLinks = 0;
	for(auto m : AppManager::getConfig().getModules()) {
		totalPins += m->getPins().size();
		for(auto p : m->getPins()) {
			totalNodes += p->getNodeGroup()->getNodes().size();
			for(auto n : p->getNodeGroup()->getNodes()) {
				for(auto& i : n->getInputs()) {
					if(!i.node.expired())
						totalLinks += 1;
				}
			}
		}
	}
	
	// Total Pins
	ImGui::TextUnformatted("Total Pins");
	ImGui::NextColumn();
	ImGui::Text("%i", totalPins);
	ImGui::NextColumn();

	// Total Nodes
	ImGui::TextUnformatted("Total Nodes");
	ImGui::NextColumn();
	ImGui::Text("%i", totalNodes);
	ImGui::NextColumn();

	// Total Links
	ImGui::TextUnformatted("Total Links");
	ImGui::NextColumn();
	ImGui::Text("%i", totalLinks);
	ImGui::NextColumn();
	*/

	// Selection stats
	ImGui::Columns(1);
	ImGui::Spacing();
	ImGui::TextUnformatted(ICON_FA_WRENCH" Selection");
	ImGui::Separator();
	ImGui::Columns(2, "###Column", true);

	std::shared_ptr<Selectable> selected = AppManager::getSelectedItem().lock();
	if(selected) {

		auto module = std::dynamic_pointer_cast<Module>(selected);
		//if(module)
			//drawStats(module);

		auto pin = std::dynamic_pointer_cast<Pin>(selected);
		//if(pin)
			//drawStats(pin);

	}

	ImGui::Columns(1);

	ImGui::End();
}
