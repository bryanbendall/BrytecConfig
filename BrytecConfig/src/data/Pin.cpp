#include "Pin.h"

namespace Brytec {

void Pin::setNodeGroup(std::shared_ptr<NodeGroup> nodeGroup)
{
    if (auto oldNodeGroup = m_nodeGroup.lock())
        oldNodeGroup->setAssigned(false);

    if (nodeGroup)
        nodeGroup->setAssigned(true);

    m_nodeGroup = nodeGroup;
}

}