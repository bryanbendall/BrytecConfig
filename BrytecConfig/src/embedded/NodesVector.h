#pragma once

#include "Nodes.h"
#include "NodeVariant.h"

namespace Embedded
{
	template<size_t SIZE>
	class NodesVector
	{

	public:
		NodesVector() {}

		bool add(NodeTypes type) 
		{
			if(m_size >= MAX_ELEMENTS)
				return false;

			m_nodeTypes[m_size] = type;

			float* nextData = &m_nodeData[m_dataNextIndex];
			NodeVariant var(type, nextData);

			if((m_dataNextIndex + var.getSize()) > MAX_DATA)
				return false;

			var.initalizeData();
			m_size++;
			m_dataNextIndex += var.getSize();
			return true;
		}

		NodeVariant at(uint32_t index) 
		{ 
			if(index >= m_size)
				return NodeVariant(NodeTypes::Count, nullptr);


			uint32_t nodeDataIndex = 0;
			for(uint32_t indexCount = 0; indexCount < index; indexCount++) {
				NodeVariant var(m_nodeTypes[indexCount], &m_nodeData[nodeDataIndex]);
				nodeDataIndex += var.getSize();
			}

			NodeVariant out(m_nodeTypes[index], &m_nodeData[nodeDataIndex]);

			return out;
		}

		NodeInputMask& getMask(uint32_t index)
		{
			return m_masks[index];
		}

		void setValue(uint32_t nodeIndex, uint8_t inputIndex, float value)
		{
			at(nodeIndex).setValue(inputIndex, getMask(nodeIndex), value);
		}

		void setInput(uint32_t nodeIndex, uint8_t inputIndex, uint32_t connectedNodeIndex, uint8_t outputIndex = 0)
		{
			at(nodeIndex).setInput(inputIndex, getMask(nodeIndex), at(connectedNodeIndex).getOutput(outputIndex));
		}

		void evaluateAll(float timestep) 
		{
			uint32_t nodeDataIndex = 0;
			for(uint32_t indexCount = 0; indexCount < m_size; indexCount++) {
				NodeVariant var(m_nodeTypes[indexCount], &m_nodeData[nodeDataIndex]);
				var.evaluate(m_masks[indexCount], timestep);
				nodeDataIndex += var.getSize();
			}
		}

	private:
		// Array max sizes
		static constexpr size_t MAX_ELEMENTS = SIZE / 4;
		static constexpr size_t MAX_DATA = SIZE - MAX_ELEMENTS;

		// Data
		NodeTypes m_nodeTypes[MAX_ELEMENTS] = {};
		NodeInputMask m_masks[MAX_ELEMENTS] = {};
		float m_nodeData[MAX_DATA] = {};

		// Vector states
		uint32_t m_size = 0;
		uint32_t m_dataNextIndex = 0;
	};

}