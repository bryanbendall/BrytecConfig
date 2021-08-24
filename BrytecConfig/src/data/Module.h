#pragma once

#include <vector>
#include "Pin.h"
#include "Selectable.h"
#include <memory>
#include <string>

enum class ModuleTypes {
	Input,
	Power,
	Count
};

class Module : public Selectable {

	std::vector<std::shared_ptr<Pin>> m_pins;
	std::string m_name = "Module";
	uint8_t m_address = 2;
	bool m_enabled = true;
	ModuleTypes m_type;

public:
	static std::string typeNames[(size_t) ModuleTypes::Count];

public:
	Module(ModuleTypes type);

	const std::vector<std::shared_ptr<Pin>>& getPins() { return m_pins; }
	std::string& getName() { return m_name; }
	uint8_t& getAddress() { return m_address; }
	bool& getEnabled() { return m_enabled; }
	ModuleTypes& getType() { return m_type; }

};

