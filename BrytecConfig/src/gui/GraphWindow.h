#pragma once

#include <imgui.h>
#include <map>
#include <set>
#include <stdint.h>
#include <string>

namespace Brytec {

class GraphWindow {

private:
    struct NodeGroupAddress {
        uint8_t moduleAddress;
        uint16_t pinAddress;

        bool operator==(const NodeGroupAddress& other)
        {
            return moduleAddress == other.moduleAddress && pinAddress == other.pinAddress;
        }

        bool operator<(const NodeGroupAddress& other) const
        {
            if (moduleAddress == other.moduleAddress)
                return pinAddress < other.pinAddress;
            else
                return moduleAddress < other.moduleAddress;
        }
    };

    struct GraphPoint{
        float time;
        float value;
        float current;
    };

    struct ScrollingBuffer {
        int MaxSize;
        int Offset;
        ImVector<GraphPoint> Data;
        ScrollingBuffer(int max_size = 500)
        {
            MaxSize = max_size;
            Offset = 0;
            Data.reserve(MaxSize);
        }
        void AddPoint(float time, float value, float current)
        {
            if (Data.size() < MaxSize)
                Data.push_back({time, value, current});
            else {
                Data[Offset] = { time, value, current };
                Offset = (Offset + 1) % MaxSize;
            }
        }
    };

public:
    GraphWindow();
    void drawWindow();
    bool getOpenedState() { return m_opened; }
    void setOpenedState(bool state) { m_opened = state; }

private:
    void addToGraphs(NodeGroupAddress addr);
    void removeFromGraphs(NodeGroupAddress addr);
    void updateBuffers(float timestep);
    std::string findAddressName(NodeGroupAddress addr);

private:
    bool m_opened = true;
    std::set<NodeGroupAddress> m_addresses;
    std::map<NodeGroupAddress, ScrollingBuffer> m_buffers;
};
}