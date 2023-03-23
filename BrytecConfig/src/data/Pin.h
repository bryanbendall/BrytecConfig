#pragma once

#include "BrytecConfigEmbedded/IOTypes.h"
#include "NodeGroup.h"
#include "Selectable.h"

namespace Brytec {

class Pin : public Selectable {

public:
    Pin() = default;
    virtual ~Pin() = default;

    void setAvailableTypes(const std::vector<IOTypes::Types>& availableTypes) { m_availableTypes = availableTypes; }
    std::vector<IOTypes::Types>& getAvailableTypes() { return m_availableTypes; }

    std::shared_ptr<NodeGroup> getNodeGroup() { return m_nodeGroup.lock(); }
    void setNodeGroup(std::shared_ptr<NodeGroup> nodeGroup, bool simulation = false);

protected:
    std::vector<IOTypes::Types> m_availableTypes;
    std::weak_ptr<NodeGroup> m_nodeGroup;
};

}