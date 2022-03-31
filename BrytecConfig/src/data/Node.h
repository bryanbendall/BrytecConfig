#pragma once

#include "BrytecConfigEmbedded/ENode.h"
#include "utils/UUID.h"
#include <imgui.h>
#include <memory>
#include <string>
#include <vector>

class Node;
struct NodeConnection {
    std::weak_ptr<Node> ConnectedNode;
    int OutputIndex = -1;
    float DefaultValue;

    NodeConnection(float defaultValue = 0.0f)
        : DefaultValue(defaultValue)
    {
    }

    NodeConnection(std::weak_ptr<Node> node, int outputIndex, float defaultValue)
        : ConnectedNode(node)
        , OutputIndex(outputIndex)
        , DefaultValue(defaultValue)
    {
    }

    bool operator==(const NodeConnection& other) const
    {
        if (!ConnectedNode.expired() && !other.ConnectedNode.expired()) {
            if (ConnectedNode.lock() == other.ConnectedNode.lock() && OutputIndex == other.OutputIndex)
                return true;
        }

        return false;
    }
};

class Node {

protected:
    Node(int id, ImVec2 position, NodeTypes type);

public:
    static std::shared_ptr<Node> create(int id, ImVec2 position, NodeTypes type);
    virtual ~Node() { }

    virtual void evaluate() { }

    std::string& getName() { return m_name; }
    void setName(std::string& name) { m_name = name; }

    ImVec2& getPosition() { return m_position; }

    NodeTypes& getType() { return m_type; }

    int getId() { return m_id; }
    int getOutputId(int index) { return (m_id << 8) + index; }
    int getIntputId(int index) { return (m_id << 8) + m_outputs.size() + index; }
    int getValueId(int index) { return (m_id << 8) + m_values.size() + m_outputs.size() + index; }

    float getOutputValue(size_t outputIndex);
    std::vector<float>& getValues() { return m_values; }
    float& getValue(int index) { return m_values[index]; }
    void setValue(int index, float value) { m_values[index] = value; }

    NodeConnection& getInput(int index) { return m_inputs[index]; }
    std::vector<NodeConnection> getInputs() { return m_inputs; }
    void setInput(int inputIndex, NodeConnection nodeConnection);
    float& getInputValue(int inputIndex);

    float& getOutput(int index) { return m_outputs[index]; }
    std::vector<float>& getOutputs() { return m_outputs; }

    const bool& getLoopFound() { return m_loopFound; }
    void setLoopFound(bool state) { m_loopFound = state; }

    UUID getSelectedNodeGroup() { return m_selectedNodeGroup; }
    void setSelectedNodeGroup(UUID nodeGroup) { m_selectedNodeGroup = nodeGroup; }

    unsigned int getBytesSize();

private:
    bool hasConnection(int inputIndex);

public:
    static const char* s_nodeName[(int)NodeTypes::Count];
    static const char* s_getTypeName(NodeTypes type) { return s_nodeName[(int)type]; }

protected:
    std::string m_name;
    NodeTypes m_type;
    int m_id;
    ImVec2 m_position;
    std::vector<float> m_outputs;
    std::vector<NodeConnection> m_inputs;
    std::vector<float> m_values;
    bool m_loopFound = false;
    UUID m_selectedNodeGroup;
};
