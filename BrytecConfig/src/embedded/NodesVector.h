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
			if(m_nextIndex > SIZE)
				return false;

			m_data[m_nextIndex] = (uint8_t) type;
			NodeVariant var(&m_data[m_nextIndex]);

			if(m_nextIndex + var.getSize() > SIZE)
				return false;

			var.initalizeData();
			m_nextIndex += var.getSize();
			m_count++;
			return true;
		}

		NodeVariant at(uint32_t index) 
		{ 
			if(index >= m_count)
				return NodeVariant(nullptr);

			uint8_t* indexIterator = m_data;
			NodeVariant var(indexIterator);

			for(uint32_t indexCount = 0; indexCount != index; indexCount++) {
				var = NodeVariant(indexIterator);
				indexIterator += var.getSize();
			}

			return var;
		}

		NodeVariant next(NodeVariant& var) 
		{
			return NodeVariant(var.pointer + var.getSize());
		}

		void evaluateAll(float timestep) 
		{
			NodeVariant v = at(0);
			v.evaluate(timestep);
			for(uint32_t i = 1; i < m_count; i++) {
				v = next(v);
				v.evaluate(timestep);
			}
		}

	private:
		uint8_t m_data[SIZE] = {};
		uint32_t m_nextIndex = 0;
		uint32_t m_count = 0;

	};

}