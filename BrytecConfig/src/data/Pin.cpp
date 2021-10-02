#include "Pin.h"
#include <iostream>

const char* Pin::currentNames[] = {
	"5 Amps",
	"7 Amps",
	"9 Amps",
	"11 Amps",
	"13 Amps",
	"15 Amps",
	"17 Amps"
};

Pin::Pin(const std::string& pinoutName, const std::vector<IOTypes::Types>& availableTypes)
	: m_pinoutName(pinoutName), m_availableTypes(availableTypes)
{
}

Pin::~Pin()
{
}

void Pin::setNodeGroup(std::shared_ptr<NodeGroup> nodeGroup)
{
	if(auto oldNodeGroup = m_nodeGroup.lock())
		oldNodeGroup->setAssigned(false);

	if(nodeGroup)
		nodeGroup->setAssigned(true);
	m_nodeGroup = nodeGroup;
}
