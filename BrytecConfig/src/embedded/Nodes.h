#pragma once

#include <stdint.h>

namespace Embedded
{

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
		float* rawData = nullptr;
		float out;

		void SetInput(uint8_t index, float* output);
		float* GetOutput(uint8_t index);
		void Evaluate(float timestep);
	};

	struct FinalValueNode
	{
		float* in = nullptr;

		void SetInput(uint8_t index, float* output);
		float* GetOutput(uint8_t index);
		void Evaluate(float timestep);
	};

	struct NodeGroupNode
	{
		float* rawData = nullptr;
		float out;

		void SetInput(uint8_t index, float* output);
		float* GetOutput(uint8_t index);
		void Evaluate(float timestep);
	};

	struct AndNode
	{
		float* in1 = nullptr;
		float* in2 = nullptr;
		float* in3 = nullptr;
		float* in4 = nullptr;
		float* in5 = nullptr;
		float out;

		void SetInput(uint8_t index, float* output);
		float* GetOutput(uint8_t index);
		void Evaluate(float timestep);
	};

	struct OrNode
	{
		float* in1 = nullptr;
		float* in2 = nullptr;
		float* in3 = nullptr;
		float* in4 = nullptr;
		float* in5 = nullptr;
		float out;

		void SetInput(uint8_t index, float* output);
		float* GetOutput(uint8_t index);
		void Evaluate(float timestep);
	};

	struct TwoStageNode
	{
		float* stage1Trigger = nullptr;
		float* stage1Percent = nullptr;
		float* stage2Trigger = nullptr;
		float* stage2Percent = nullptr;
		float out;

		void SetInput(uint8_t index, float* output);
		float* GetOutput(uint8_t index);
		void Evaluate(float timestep);
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
		float* in = nullptr;
		float* repeat = nullptr;
		float* timeout = nullptr;
		Types curveType = Types::Toggle;
		float timerCounter = 0.0f;
		float out;

		void SetInput(uint8_t index, float* output);
		float* GetOutput(uint8_t index);
		void Evaluate(float timestep);
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
		float* input1 = nullptr;
		float* input2 = nullptr;
		Types compareType;
		float out;

		void SetInput(uint8_t index, float* output);
		float* GetOutput(uint8_t index);
		void Evaluate(float timestep);
	};

	struct OnOffNode
	{
		float* on = nullptr;
		float* off = nullptr;
		float out;

		void SetInput(uint8_t index, float* output);
		float* GetOutput(uint8_t index);
		void Evaluate(float timestep);
	};

	struct InvertNode
	{
		float* in = nullptr;
		float out;

		void SetInput(uint8_t index, float* output);
		float* GetOutput(uint8_t index);
		void Evaluate(float timestep);
	};

	struct ToggleNode
	{
		float* in = nullptr;
		bool lastValue;
		float out;

		void SetInput(uint8_t index, float* output);
		float* GetOutput(uint8_t index);
		void Evaluate(float timestep);
	};

	struct DelayNode
	{
		float* in = nullptr;
		float* delayTime = nullptr;
		float counter = 0.0f;
		float out;

		void SetInput(uint8_t index, float* output);
		float* GetOutput(uint8_t index);
		void Evaluate(float timestep);
	};

	struct PushButtonNode
	{
		float* button = nullptr;
		float* neutralSafety = nullptr;
		float* engineRunning = nullptr;
		bool lastButtonState = false;
		float ignitionOut;
		float starterOut;

		void SetInput(uint8_t index, float* output);
		float* GetOutput(uint8_t index);
		void Evaluate(float timestep);
	};

	struct MapValueNode
	{
		float* in = nullptr;
		float* fromMin = nullptr;
		float* fromMax = nullptr;
		float* toMin = nullptr;
		float* toMax = nullptr;
		float out;

		void SetInput(uint8_t index, float* output);
		float* GetOutput(uint8_t index);
		void Evaluate(float timestep);
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
		float* input1 = nullptr;
		float* input2 = nullptr;
		Types mathType;
		float out;

		void SetInput(uint8_t index, float* output);
		float* GetOutput(uint8_t index);
		void Evaluate(float timestep);
	};

	struct ValueNode
	{
		float out;

		void SetInput(uint8_t index, float* output);
		float* GetOutput(uint8_t index);
		void Evaluate(float timestep);
	};

	struct SwitchNode
	{
		float* selection = nullptr;
		float* input1 = nullptr;
		float* input2 = nullptr;
		float out;

		void SetInput(uint8_t index, float* output);
		float* GetOutput(uint8_t index);
		void Evaluate(float timestep);
	};

}