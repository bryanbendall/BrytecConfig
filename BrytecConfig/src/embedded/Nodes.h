#pragma once

#include <stdint.h>

namespace Embedded
{

	struct AndNode
	{
		float* in1 = nullptr;
		float* in2 = nullptr;
		float* in3 = nullptr;
		float* in4 = nullptr;
		float* in5 = nullptr;
		float out;
	};

	struct OrNode
	{
		float* in1 = nullptr;
		float* in2 = nullptr;
		float* in3 = nullptr;
		float* in4 = nullptr;
		float* in5 = nullptr;
		float out;
	};

	struct TwoStageNode
	{
		float* stage1Trigger = nullptr;
		float* stage2Trigger = nullptr;
		uint8_t stage1Percent = 0;
		uint8_t stage2Percent = 50;
		float out;
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
		Types curveType = Types::Toggle;
		bool repeat = false;
		bool onShutdown = false;
		bool lastIn = false;
		float timeout = 0.0f;
		float timerCounter = 0.0f;
		float out;
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
	};

	struct OnOffNode
	{
		float* on = nullptr;
		float* off = nullptr;
		float out;
	};

	struct InvertNode
	{
		float* in = nullptr;
		float out;
	};

	struct ToggleNode
	{
		float* in = nullptr;
		bool lastValue;
		float out;
	};

	struct DelayNode
	{
		float* in = nullptr;
		float delayTime = 0.0f;
		float counter = 0.0f;
		float out;
	};

	struct PushButtonNode
	{
		float* button = nullptr;
		float* neutralSafety = nullptr;
		float* engineRunning = nullptr;
		bool lastButtonState = false;
		float ignitionOut;
		float starterOut;
	};

	struct MapValueNode
	{
		float* in = nullptr;
		float fromMin = 0.0f;
		float toMin = 0.0f;
		float fromMax = 0.0f;
		float toMax = 0.0f;
		float out;
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
	};

	struct ValueNode
	{
		float out;
	};


	void Evaluate(AndNode& node, float timestep);
	void Evaluate(OrNode& node, float timestep);
	void Evaluate(TwoStageNode& node, float timestep);
	void Evaluate(CurveNode& node, float timestep);
	void Evaluate(CompareNode& node, float timestep);
	void Evaluate(OnOffNode& node, float timestep);
	void Evaluate(InvertNode& node, float timestep);
	void Evaluate(ToggleNode& node, float timestep);
	void Evaluate(DelayNode& node, float timestep);
	void Evaluate(PushButtonNode& node, float timestep);
	void Evaluate(MapValueNode& node, float timestep);
	void Evaluate(MathNode& node, float timestep);
	void Evaluate(ValueNode& node, float timestep);

}