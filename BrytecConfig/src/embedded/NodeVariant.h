#pragma once

#include "Nodes.h"

namespace Embedded
{
	class NodeVariant
	{
	public:
		NodeVariant(NodeTypes type, float* ptr);

		void initalizeData();
		uint32_t getSize();
		
		void setInput(uint8_t index, NodeInputMask& mask, float* output);
		void setValue(uint8_t index, NodeInputMask& mask, float value);
		
		float* getOutput(uint8_t index = 0);
		
		void evaluate(NodeInputMask& mask, float timestep);

	private:
		NodeTypes m_type;
		float* m_data;
	};

}