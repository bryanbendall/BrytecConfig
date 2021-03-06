#pragma once

#include "Module.h"
#include "NodeGroup.h"
#include <filesystem>
#include <memory>
#include <vector>

class Config {

public:
    Config(const std::filesystem::path& path);

    void addModule(std::filesystem::path modulePath);
    void addModule(std::shared_ptr<Module>& module);
    void removeModule(std::shared_ptr<Module>& module);
    const std::vector<std::shared_ptr<Module>>& getModules() { return m_modules; }

    std::shared_ptr<NodeGroup> addNodeGroup();
    void addNodeGroup(std::shared_ptr<NodeGroup> nodeGroup) { m_nodeGroups.push_back(nodeGroup); }
    std::shared_ptr<NodeGroup> addEmptyNodeGroup(UUID uuid);
    void removeNodeGroup(std::shared_ptr<NodeGroup>& nodeGroup);
    const std::vector<std::shared_ptr<NodeGroup>>& getNodeGroups() { return m_nodeGroups; }
    const std::shared_ptr<NodeGroup> findNodeGroup(UUID uuid);

    const std::filesystem::path& getFilepath() { return m_path; }
    void setFilepath(const std::filesystem::path& path) { m_path = path; }

private:
    std::filesystem::path m_path;
    std::vector<std::shared_ptr<Module>> m_modules;
    std::vector<std::shared_ptr<NodeGroup>> m_nodeGroups;
};
