#pragma once

#include <stdint.h>

namespace Embedded
{

	class NodeInputMask
	{
		uint8_t m_mask;

	public:
		void setPointer(uint8_t index) { m_mask |= (1 << index); }
		void setValue(uint8_t index) { m_mask &= ~(1 << index); }
		bool isPointer(uint8_t index) { return m_mask & (1 << index); }
	};

	union InputOrValue
	{
		float value;
		float* pointer;
	};

	enum class NodeTypes : uint8_t
	{
		Final_Value = 0,
		Initial_Value,
		Node_Group, // TODO
		And,
		Or,
		Two_Stage,
		Curve,
		Compare,
		On_Off,
		Invert,
		Toggle,
		Delay,
		Push_Button,
		Map_Value,
		Math,
		Value,
		Switch,
		Count
	};

	struct InitalValueNode
	{
		InputOrValue m_rawData;
		float m_out;

		void SetInput(uint8_t index, NodeInputMask& mask, float* output);
		void SetValue(uint8_t index, NodeInputMask& mask, float value);
		float* GetOutput(uint8_t index);
		void Evaluate(NodeInputMask& mask, float timestep);
	};

	struct FinalValueNode
	{
		InputOrValue m_in;

		void SetInput(uint8_t index, NodeInputMask& mask, float* output);
		void SetValue(uint8_t index, NodeInputMask& mask, float value);
		float* GetOutput(uint8_t index);
		void Evaluate(NodeInputMask& mask, float timestep);
	};

	struct NodeGroupNode
	{
		InputOrValue m_rawData;
		float m_out;

		void SetInput(uint8_t index, NodeInputMask& mask, float* output);
		void SetValue(uint8_t index, NodeInputMask& mask, float value);
		float* GetOutput(uint8_t index);
		void Evaluate(NodeInputMask& mask, float timestep);
	};

	struct AndNode
	{
		InputOrValue m_in1 = {1.0f};
		InputOrValue m_in2 = {1.0f};
		InputOrValue m_in3 = {1.0f};
		InputOrValue m_in4 = {1.0f};
		InputOrValue m_in5 = {1.0f};
		float m_out;

		void SetInput(uint8_t index, NodeInputMask& mask, float* output);
		void SetValue(uint8_t index, NodeInputMask& mask, float value);
		float* GetOutput(uint8_t index);
		void Evaluate(NodeInputMask& mask, float timestep);
	};

	struct OrNode
	{
		InputOrValue m_in1;
		InputOrValue m_in2;
		InputOrValue m_in3;
		InputOrValue m_in4;
		InputOrValue m_in5;
		float m_out;

		void SetInput(uint8_t index, NodeInputMask& mask, float* output);
		void SetValue(uint8_t index, NodeInputMask& mask, float value);
		float* GetOutput(uint8_t index);
		void Evaluate(NodeInputMask& mask, float timestep);
	};

	struct TwoStageNode
	{
		InputOrValue m_stage1Trigger;
		InputOrValue m_stage1Percent;
		InputOrValue m_stage2Trigger;
		InputOrValue m_stage2Percent;
		float m_out;

		void SetInput(uint8_t index, NodeInputMask& mask, float* output);
		void SetValue(uint8_t index, NodeInputMask& mask, float value);
		float* GetOutput(uint8_t index);
		void Evaluate(NodeInputMask& mask, float timestep);
	};

	struct CurveNode
	{
		enum class Types : uint8_t
		{
			Toggle,
			Linear,
			Exponential,
			Breathing,
			Count
		};
		InputOrValue m_in;
		InputOrValue m_repeat;
		InputOrValue m_timeout;
		InputOrValue m_curveType;
		InputOrValue m_timerCounter;
		float m_out;

		void SetInput(uint8_t index, NodeInputMask& mask, float* output);
		void SetValue(uint8_t index, NodeInputMask& mask, float value);
		float* GetOutput(uint8_t index);
		void Evaluate(NodeInputMask& mask, float timestep);
	};

	struct CompareNode
	{
		enum class Types : uint8_t
		{
			Greater,
			GreaterEqual,
			Less,
			LessEqual,
			Count
		};
		InputOrValue m_input1;
		InputOrValue m_input2;
		InputOrValue m_compareType;
		float m_out;

		void SetInput(uint8_t index, NodeInputMask& mask, float* output);
		void SetValue(uint8_t index, NodeInputMask& mask, float value);
		float* GetOutput(uint8_t index);
		void Evaluate(NodeInputMask& mask, float timestep);
	};

	struct OnOffNode
	{
		InputOrValue m_on;
		InputOrValue m_off;
		float m_out;

		void SetInput(uint8_t index, NodeInputMask& mask, float* output);
		void SetValue(uint8_t index, NodeInputMask& mask, float value);
		float* GetOutput(uint8_t index);
		void Evaluate(NodeInputMask& mask, float timestep);
	};

	struct InvertNode
	{
		InputOrValue m_in;
		float m_out;

		void SetInput(uint8_t index, NodeInputMask& mask, float* output);
		void SetValue(uint8_t index, NodeInputMask& mask, float value);
		float* GetOutput(uint8_t index);
		void Evaluate(NodeInputMask& mask, float timestep);
	};

	struct ToggleNode
	{
		InputOrValue m_in;
		InputOrValue m_lastValue;
		float m_out;

		void SetInput(uint8_t index, NodeInputMask& mask, float* output);
		void SetValue(uint8_t index, NodeInputMask& mask, float value);
		float* GetOutput(uint8_t index);
		void Evaluate(NodeInputMask& mask, float timestep);
	};

	struct DelayNode
	{
		InputOrValue m_in;
		InputOrValue m_delayTime;
		InputOrValue m_counter;
		float m_out;

		void SetInput(uint8_t index, NodeInputMask& mask, float* output);
		void SetValue(uint8_t index, NodeInputMask& mask, float value);
		float* GetOutput(uint8_t index);
		void Evaluate(NodeInputMask& mask, float timestep);
	};

	struct PushButtonNode
	{
		InputOrValue m_button;
		InputOrValue m_neutralSafety;
		InputOrValue m_engineRunning;
		InputOrValue m_lastButtonState;
		float m_ignitionOut;
		float m_starterOut;

		void SetInput(uint8_t index, NodeInputMask& mask, float* output);
		void SetValue(uint8_t index, NodeInputMask& mask, float value);
		float* GetOutput(uint8_t index);
		void Evaluate(NodeInputMask& mask, float timestep);
	};

	struct MapValueNode
	{
		InputOrValue m_in;
		InputOrValue m_fromMin;
		InputOrValue m_fromMax;
		InputOrValue m_toMin;
		InputOrValue m_toMax;
		float m_out;

		void SetInput(uint8_t index, NodeInputMask& mask, float* output);
		void SetValue(uint8_t index, NodeInputMask& mask, float value);
		float* GetOutput(uint8_t index);
		void Evaluate(NodeInputMask& mask, float timestep);
	};

	struct MathNode
	{
		enum class Types : uint8_t
		{
			Add,
			Subtract,
			Multiply,
			Divide,
			Count
		};
		InputOrValue m_input1;
		InputOrValue m_input2;
		InputOrValue m_mathType;
		float m_out;

		void SetInput(uint8_t index, NodeInputMask& mask, float* output);
		void SetValue(uint8_t index, NodeInputMask& mask, float value);
		float* GetOutput(uint8_t index);
		void Evaluate(NodeInputMask& mask, float timestep);
	};

	struct ValueNode
	{
		float m_out;

		void SetInput(uint8_t index, NodeInputMask& mask, float* output);
		void SetValue(uint8_t index, NodeInputMask& mask, float value);
		float* GetOutput(uint8_t index);
		void Evaluate(NodeInputMask& mask, float timestep);
	};

	struct SwitchNode
	{
		InputOrValue m_selection;
		InputOrValue m_input1;
		InputOrValue m_input2;
		float m_out;

		void SetInput(uint8_t index, NodeInputMask& mask, float* output);
		void SetValue(uint8_t index, NodeInputMask& mask, float value);
		float* GetOutput(uint8_t index);
		void Evaluate(NodeInputMask& mask, float timestep);
	};

}