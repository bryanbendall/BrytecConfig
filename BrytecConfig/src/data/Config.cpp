#include "Config.h"

#include "Module.h"
#include "utils/ModuleSerializer.h"

Config::Config(const std::filesystem::path& path)
    : m_path(path)
{
}

void Config::addModule(std::filesystem::path modulePath)
{
    std::shared_ptr<Module> module = std::make_shared<Module>();
    ModuleSerializer serializer(module);
    BinaryDeserializer des;
    des.setDataFromPath(modulePath);
    if (serializer.deserializeTemplateBinary(des)) {
        m_modules.push_back(module);
    }
}

void Config::addModule(std::shared_ptr<Module>& module)
{
    m_modules.push_back(module);
}

std::shared_ptr<NodeGroup> Config::addNodeGroup()
{
    std::shared_ptr<NodeGroup> nodeGroup = std::make_shared<NodeGroup>();
    m_nodeGroups.push_back(nodeGroup);
    return nodeGroup;
}

std::shared_ptr<NodeGroup> Config::addEmptyNodeGroup(UUID uuid)
{
    std::shared_ptr<NodeGroup> nodeGroup = std::make_shared<NodeGroup>(uuid);
    m_nodeGroups.push_back(nodeGroup);
    return nodeGroup;
}

void Config::removeModule(std::shared_ptr<Module>& module)
{
    auto it = std::find(m_modules.begin(), m_modules.end(), module);
    m_modules.erase(it);
}

void Config::removeNodeGroup(std::shared_ptr<NodeGroup>& nodeGroup)
{
    auto it = std::find(m_nodeGroups.begin(), m_nodeGroups.end(), nodeGroup);
    m_nodeGroups.erase(it);
}

const std::shared_ptr<NodeGroup> Config::findNodeGroup(UUID uuid)
{
    for (auto& nodeGroup : m_nodeGroups) {
        if (nodeGroup->getId() == uuid)
            return nodeGroup;
    }

    return nullptr;
}

uint8_t Config::getAssignedModuleAddress(std::shared_ptr<NodeGroup> nodeGroup)
{
    for (auto module : m_modules) {

        for (auto pin : module->getPhysicalPins()) {
            if (pin->getNodeGroup() == nodeGroup)
                return module->getAddress();
        }

        for (auto pin : module->getInternalPins()) {
            if (pin->getNodeGroup() == nodeGroup)
                return module->getAddress();
        }
    }

    return 0;
}

uint8_t Config::getAssignedPinAddress(std::shared_ptr<NodeGroup> nodeGroup)
{
    for (auto module : m_modules) {

        for (int i = 0; i < module->getPhysicalPins().size(); i++) {
            if (module->getPhysicalPins()[i]->getNodeGroup() == nodeGroup)
                return i;
        }

        for (int i = 0; i < module->getInternalPins().size(); i++) {

            if (module->getInternalPins()[i]->getNodeGroup() == nodeGroup)
                return i + module->getPhysicalPins().size();
        }
    }

    return 0;
}

std::shared_ptr<Pin> Config::getAssignedPin(std::shared_ptr<NodeGroup> nodeGroup)
{
    for (auto module : m_modules) {

        for (int i = 0; i < module->getPhysicalPins().size(); i++) {
            if (module->getPhysicalPins()[i]->getNodeGroup() == nodeGroup)
                return module->getPhysicalPins()[i];
        }

        for (int i = 0; i < module->getInternalPins().size(); i++) {

            if (module->getInternalPins()[i]->getNodeGroup() == nodeGroup)
                return module->getInternalPins()[i];
        }
    }

    return nullptr;
}