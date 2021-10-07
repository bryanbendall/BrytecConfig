#pragma once

#include <vector>
#include "Pin.h"
#include "Selectable.h"
#include <memory>
#include <string>

class Module : public Selectable {

public:
	Module();
	
	std::string& getName() { return m_name; }
	void setName(const std::string& name) { m_name = name; }
	
	uint8_t& getAddress() { return m_address; }
	void setAddress(const uint8_t& address) { m_address = address; }
	
	bool& getEnabled() { return m_enabled; }
	void setEnabled(bool state) { m_enabled = state; }
	
	void addPin() { m_pins.push_back(std::make_shared<Pin>("Unnamed", std::vector<IOTypes::Types>())); }
	void addPin(std::shared_ptr<Pin>& pin) { m_pins.push_back(pin); }
	void addPin(std::string name, std::initializer_list<IOTypes::Types> types) { m_pins.push_back(std::make_shared<Pin>(name, types)); }
	void deletePin(std::shared_ptr<Pin>& pin);
	std::vector<std::shared_ptr<Pin>>& getPins() { return m_pins; }

private:
	std::vector<std::shared_ptr<Pin>> m_pins;
	std::string m_name = "Unnamed";
	uint8_t m_address = 2;
	bool m_enabled = true;

};

