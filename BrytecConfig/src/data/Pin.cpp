#include "Pin.h"

void Pin::setNodeGroup(std::shared_ptr<NodeGroup> nodeGroup, bool simulation)
{
    if (auto oldNodeGroup = m_nodeGroup.lock())
        oldNodeGroup->setAssigned(false);

    if (nodeGroup && !simulation)
        nodeGroup->setAssigned(true);

    m_nodeGroup = nodeGroup;
}
