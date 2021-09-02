#pragma once

#include <vector>
#include "Pin.h"
#include "Selectable.h"
#include <memory>
#include <string>

class Module : public Selectable {

	std::vector<std::shared_ptr<Pin>> m_pins;
	std::string m_name = "Module";
	uint8_t m_address = 2;
	bool m_enabled = true;

public:
	Module();

	std::vector<std::shared_ptr<Pin>>& getPins() { return m_pins; }
	std::string& getName() { return m_name; }
	uint8_t& getAddress() { return m_address; }
	bool& getEnabled() { return m_enabled; }

};

