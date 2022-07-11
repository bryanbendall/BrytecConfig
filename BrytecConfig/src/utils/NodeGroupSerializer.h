#pragma once

#include "AppManager.h"
#include "BrytecConfigEmbedded/Utils/BinaryDeserializer.h"
#include "data/NodeGroup.h"
#include "utils/BinarySerializer.h"
#include <filesystem>
#include <memory>
#include <yaml-cpp/yaml.h>

class NodeGroupSerializer {

public:
    NodeGroupSerializer(std::shared_ptr<NodeGroup>& nodeGroup);

    BinarySerializer serializeBinary();
    bool deserializeBinary(BinaryDeserializer& des);

private:
    std::shared_ptr<NodeGroup> m_nodeGroup;

    friend class ConfigSerializer;
};