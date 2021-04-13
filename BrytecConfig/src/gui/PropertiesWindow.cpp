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
	ImGui::Columns(2, "###PropColumn", true);
	ImGui::SetColumnWidth(-1, 100.0f);
	ImGui::TextUnformatted("Name");
	ImGui::NextColumn();
	ImGui::SetNextItemWidth(-FLT_MIN);
	ImGui::InputText("###Name", &module->m_name);
	ImGui::NextColumn();
	ImGui::TextUnformatted("Address");
	ImGui::NextColumn();
	static bool showButtons = true;
	ImGui::SetNextItemWidth(-FLT_MIN);
	ImGui::InputScalar("###addr", ImGuiDataType_U8, &module->m_address, &showButtons);
	ImGui::NextColumn();
	ImGui::TextUnformatted("Module Type");
	ImGui::NextColumn();
	ImGui::SetNextItemWidth(-FLT_MIN);
	ImGui::InputText("###modType", &Module::typeNames[(size_t)module->m_type], ImGuiInputTextFlags_ReadOnly);
	ImGui::NextColumn();
	ImGui::TextUnformatted("Enabled");
	ImGui::NextColumn();
	ImGui::SetNextItemWidth(-FLT_MIN);
	ImGui::Checkbox("###Enabled", &module->m_enabled);
	ImGui::Columns(1);
}

void PropertiesWindow::drawPinProps(std::shared_ptr<Pin> pin)
{
	ImGui::Columns(2, "###PinColumn", true);
	ImGui::SetColumnWidth(-1, 100.0f);
	ImGui::TextUnformatted("Name");
	ImGui::NextColumn();
	ImGui::SetNextItemWidth(-FLT_MIN);
	ImGui::InputText("###Name", &pin->m_name);
	ImGui::NextColumn();
	ImGui::TextUnformatted("Pinout");
	ImGui::NextColumn();
	ImGui::SetNextItemWidth(-FLT_MIN);
	ImGui::InputText("###pinout", &pin->m_pinoutName, ImGuiInputTextFlags_ReadOnly);
	ImGui::NextColumn();
	ImGui::TextUnformatted("Enabled");
	ImGui::NextColumn();
	ImGui::SetNextItemWidth(-FLT_MIN);
	ImGui::Checkbox("###Enabled", &pin->m_enabled);
	ImGui::NextColumn();
	ImGui::TextUnformatted("Pin Type");
	ImGui::NextColumn();
	ImGui::SetNextItemWidth(-FLT_MIN);
	if(ImGui::BeginCombo("###pintype", (int) pin->m_pinType >= 0 ? Pin::typeNames[(size_t) pin->m_pinType] : " ")) {
		for(auto& type : pin->m_availableTypes) {
			ImGui::PushID(&pin);
			bool isSelected = type == pin->m_pinType;
			if(ImGui::Selectable(Pin::typeNames[(size_t) type], isSelected))
				pin->m_pinType = type;
			ImGui::PopID();

			if(isSelected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}
	ImGui::NextColumn();
	ImGui::TextUnformatted("Current Limit");
	ImGui::NextColumn();
	ImGui::SetNextItemWidth(-FLT_MIN);
	ImGui::Combo("###Current Limit", (int*)&pin->m_currentLimit, Pin::currentNames, IM_ARRAYSIZE(Pin::currentNames));
	ImGui::Columns(1);

	ImGui::Separator();
	for(auto n : pin->m_nodes) {
		ImGui::LabelText(n->m_name.c_str(), "%i", n->m_id);
	}
}

void PropertiesWindow::drawStats(std::shared_ptr<Module> module) 
{
	ImGui::TextUnformatted("Module Nodes");
	ImGui::NextColumn();
	ImGui::SetNextItemWidth(-FLT_MIN);
	int totalNodes = 0;
	for(auto pin : module->m_pins)
		totalNodes += pin->m_nodes.size();
	ImGui::Text("%i", totalNodes);
	ImGui::NextColumn();

	int totalBytes = 0;
	for(auto p : module->m_pins) {
		for(auto n : p->m_nodes) 
			totalBytes += n->getSize();
	}
	ImGui::TextUnformatted("Bytes in module");
	ImGui::NextColumn();
	ImGui::Text("%i %s", totalBytes, "bytes");
	ImGui::NextColumn();
	ImGui::TextUnformatted("Memory used");
	ImGui::NextColumn();
	ImGui::Text("%.2f%s", ((float) totalBytes / 15000.0f * 100.0f), "% used");
	ImGui::NextColumn();
}

void PropertiesWindow::drawStats(std::shared_ptr<Pin> pin)
{
	ImGui::TextUnformatted("Pin Nodes");
	ImGui::NextColumn();
	ImGui::SetNextItemWidth(-FLT_MIN);
	ImGui::Text("%i", pin->m_nodes.size());
	ImGui::NextColumn();

	int totalBytes = 0;
	for(auto n : pin->m_nodes)
		totalBytes += n->getSize();
	ImGui::TextUnformatted("Bytes in pin");
	ImGui::NextColumn();
	ImGui::Text("%i %s", totalBytes, "bytes");
	ImGui::NextColumn();
	ImGui::TextUnformatted("Memory used");
	ImGui::NextColumn();
	ImGui::Text("%.2f%s", ((float)totalBytes / 15000.0f * 100.0f), "% used");
	ImGui::NextColumn();
}

void PropertiesWindow::drawStatsWindow() 
{
	ImGui::Begin(ICON_FA_INFO_CIRCLE" Stats");
	ImGui::TextUnformatted(ICON_FA_INFO_CIRCLE" Total Stats");
	ImGui::Separator();
	ImGui::Columns(2, "###Column", true);
	ImGui::SetColumnWidth(-1, 100.0f);

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

	ImGui::TextUnformatted("Total Modules");
	ImGui::NextColumn();
	ImGui::Text("%i", AppManager::getConfig().m_modules.size());
	ImGui::NextColumn();

	int totalPins = 0;
	int totalNodes = 0;
	int totalLinks = 0;
	for(auto m : AppManager::getConfig().m_modules) {
		totalPins += m->m_pins.size();
		for(auto p : m->m_pins) {
			totalNodes += p->m_nodes.size();
			for(auto n : p->m_nodes) {
				for(auto i : n->m_inputs) {
					if(!i.node.expired())
						totalLinks += 1;
				}
			}
		}
	}
	ImGui::TextUnformatted("Total Pins");
	ImGui::NextColumn();
	ImGui::Text("%i", totalPins);
	ImGui::NextColumn();
	ImGui::TextUnformatted("Total Nodes");
	ImGui::NextColumn();
	ImGui::Text("%i", totalNodes);
	ImGui::NextColumn();
	ImGui::TextUnformatted("Total Links");
	ImGui::NextColumn();
	ImGui::Text("%i", totalLinks);
	ImGui::NextColumn();

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
