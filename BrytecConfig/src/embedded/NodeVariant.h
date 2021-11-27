#pragma once

#include "Nodes.h"

namespace Embedded
{
	struct NodeVariant
	{
		NodeVariant(uint8_t* ptr);

		void initalizeData();
		uint32_t getSize();
		void evaluate(float timestep);
		float* getOutput(uint8_t index = 0);
		void setInput(uint8_t index, float* output);

		uint8_t* pointer;
		NodeTypes type;
		void* data;
	};
}