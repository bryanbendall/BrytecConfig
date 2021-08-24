#include "PropertiesWindow.h"

#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>
#include "../AppManager.h"
#include <IconsFontAwesome5.h>
#include <memory>

void PropertiesWindow::drawWindow() {
	if (!m_opened)
		return;
	ImGui::Begin(ICON_FA_COG" Properties", &m_opened);

	std::shared_ptr<Selectable> selected = AppManager::getSelectedItem().lock();
	if (selected) {
		
		auto module = std::dynamic_pointer_cast<Module>(selected);
		if (module)
			drawModuleProps(module);

		auto pin = std::dynamic_pointer_cast<Pin>(selected);
		if (pin)
			drawPinProps(pin);
		
	}

	ImGui::End();

	drawStatsWindow();
}

void PropertiesWindow::drawModuleProps(std::shared_ptr<Module> module)
{
	// Setup Columns
	ImGui::Columns(2, "###PropColumn", true);
	ImGui::SetColumnWidth(-1, 100.0f);

	// Name
	ImGui::TextUnformatted("Name");
	ImGui::NextColumn();
	ImGui::SetNextItemWidth(-FLT_MIN);
	ImGui::InputText("###Name", &module->getName());
	ImGui::NextColumn();

	// Address
	ImGui::TextUnformatted("Address");
	ImGui::NextColumn();
	static bool showButtons = true;
	ImGui::SetNextItemWidth(-FLT_MIN);
	ImGui::InputScalar("###addr", ImGuiDataType_U8, &module->getAddress(), &showButtons);
	ImGui::NextColumn();

	// Module Type
	ImGui::TextUnformatted("Module Type");
	ImGui::NextColumn();
	ImGui::SetNextItemWidth(-FLT_MIN);
	ImGui::InputText("###modType", &Module::typeNames[(size_t)module->getType()], ImGuiInputTextFlags_ReadOnly);
	ImGui::NextColumn();

	// Enabled
	ImGui::TextUnformatted("Enabled");
	ImGui::NextColumn();
	ImGui::SetNextItemWidth(-FLT_MIN);
	ImGui::Checkbox("###Enabled", &module->getEnabled());

	// End Columns
	ImGui::Columns(1);
}

void PropertiesWindow::drawPinProps(std::shared_ptr<Pin> pin)
{
	// Setup Columns
	ImGui::Columns(2, "###PinColumn", true);
	ImGui::SetColumnWidth(-1, 100.0f);

	// Name
	ImGui::TextUnformatted("Name");
	ImGui::NextColumn();
	ImGui::SetNextItemWidth(-FLT_MIN);
	ImGui::InputText("###Name", &pin->getName());
	ImGui::NextColumn();

	// Pinout
	ImGui::TextUnformatted("Pinout");
	ImGui::NextColumn();
	ImGui::SetNextItemWidth(-FLT_MIN);
	ImGui::InputText("###pinout", &pin->getPinoutName(), ImGuiInputTextFlags_ReadOnly);
	ImGui::NextColumn();

	// Enabled
	ImGui::TextUnformatted("Enabled");
	ImGui::NextColumn();
	ImGui::SetNextItemWidth(-FLT_MIN);
	ImGui::Checkbox("###Enabled", &pin->getEnabled());
	ImGui::NextColumn();

	// Pin Type
	ImGui::TextUnformatted("Pin Type");
	ImGui::NextColumn();
	ImGui::SetNextItemWidth(-FLT_MIN);
	if(ImGui::BeginCombo("###pintype", (int) pin->getType() >= 0 ? Pin::typeNames[(size_t) pin->getType()] : " ")) {
		for(auto& type : pin->getAvailableTypes()) {
			ImGui::PushID(&pin);
			bool isSelected = type == pin->getType();
			if(ImGui::Selectable(Pin::typeNames[(size_t) type], isSelected))
				pin->getType() = type;
			ImGui::PopID();

			if(isSelected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}
	ImGui::NextColumn();

	// Current Limit
	ImGui::TextUnformatted("Current Limit");
	ImGui::NextColumn();
	ImGui::SetNextItemWidth(-FLT_MIN);
	ImGui::Combo("###Current Limit", (int*)&pin->getCurrentLimit(), Pin::currentNames, IM_ARRAYSIZE(Pin::currentNames));
	
	// End Columns
	ImGui::Columns(1);

	// Nodes
	ImGui::Separator();
	for(auto n : pin->getNodes()) {
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
		totalNodes += pin->getNodes().size();
	ImGui::Text("%i", totalNodes);
	ImGui::NextColumn();

	// Bytes in module
	int totalBytes = 0;
	for(auto p : module->getPins()) {
		for(auto n : p->getNodes())
			totalBytes += n->getSize();
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
	ImGui::Text("%i", pin->getNodes().size());
	ImGui::NextColumn();

	// Bytes in pin
	int totalBytes = 0;
	for(auto n : pin->getNodes())
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

	// Calculating number of objects
	int totalPins = 0;
	int totalNodes = 0;
	int totalLinks = 0;
	for(auto m : AppManager::getConfig().getModules()) {
		totalPins += m->getPins().size();
		for(auto p : m->getPins()) {
			totalNodes += p->getNodes().size();
			for(auto n : p->getNodes()) {
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

	// Selection stats
	ImGui::Columns(1);
	ImGui::Spacing();
	ImGui::TextUnformatted(ICON_FA_WRENCH" Selection");
	ImGui::Separator();
	ImGui::Columns(2, "###Column", true);

	std::shared_ptr<Selectable> selected = AppManager::getSelectedItem().lock();
	if(selected) {

		auto module = std::dynamic_pointer_cast<Module>(selected);
		if(module)
			drawStats(module);

		auto pin = std::dynamic_pointer_cast<Pin>(selected);
		if(pin)
			drawStats(pin);

	}

	ImGui::Columns(1);

	ImGui::End();
}
