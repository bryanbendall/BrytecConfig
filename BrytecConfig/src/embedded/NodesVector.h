#pragma once

#include "Config.h"
#include "Nodes.h"

#ifndef NODE_VECTOR_MAX_SIZE
#error Please define NODE_VECTOR_MAX_SIZE in bytes
#endif

namespace Embedded
{
	struct NodeVariant
	{
		NodeVariant(uint8_t* ptr);
		void initalizeData();
		uint32_t getSize();
		void evaluate();

		uint8_t* pointer;
		NodeTypes type;
		void* data;
	};


	class NodesVector
	{

	public:
		NodesVector();

		bool add(NodeTypes type);
		NodeVariant at(uint32_t index);
		NodeVariant next(NodeVariant& var);
		void evaluateAll();

	private:
		uint8_t m_data[NODE_VECTOR_MAX_SIZE];
		uint32_t m_nextIndex = 0;
		uint32_t m_count = 0;

	};

}