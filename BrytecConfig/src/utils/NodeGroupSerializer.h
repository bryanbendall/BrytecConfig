#pragma once

#include "AppManager.h"
#include "BrytecConfigEmbedded/Deserializer/BinaryDeserializer.h"
#include "data/NodeGroup.h"
#include "utils/BinarySerializer.h"
#include <filesystem>
#include <memory>
#include <yaml-cpp/yaml.h>

namespace Brytec {

class NodeGroupSerializer {

public:
    NodeGroupSerializer(std::shared_ptr<NodeGroup>& nodeGroup);

    BinarySerializer serializeBinary();
    bool deserializeBinary(BinaryDeserializer& des);

private:
    std::shared_ptr<NodeGroup> m_nodeGroup;
};

}