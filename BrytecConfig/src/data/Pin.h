#pragma once

#include "BrytecConfigEmbedded/IOTypes.h"
#include "NodeGroup.h"
#include "Selectable.h"
#include <deque>
#include <memory>
#include <string>

enum class PinCurrentLimit {
    V5,
    V7,
    V9,
    V11,
    V13,
    V15,
    V17,
    Count
};

class Pin : public Selectable {

public:
    Pin(const std::string& pinoutName, const std::vector<IOTypes::Types>& availableTypes);
    ~Pin();

    std::string& getPinoutName() { return m_pinoutName; }
    PinCurrentLimit& getCurrentLimit() { return m_currentLimit; }
    std::vector<IOTypes::Types>& getAvailableTypes() { return m_availableTypes; }

    std::shared_ptr<NodeGroup> getNodeGroup() { return m_nodeGroup.lock(); }
    void setNodeGroup(std::shared_ptr<NodeGroup> nodeGroup, bool simulation = false);

public:
    static const char* currentNames[(size_t)PinCurrentLimit::Count];

private:
    std::string m_pinoutName = "A1";
    PinCurrentLimit m_currentLimit = (PinCurrentLimit)-1;
    std::vector<IOTypes::Types> m_availableTypes;
    std::weak_ptr<NodeGroup> m_nodeGroup;
};
