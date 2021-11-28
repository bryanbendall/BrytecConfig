#include "Nodes.h"

#include <math.h>

namespace Embedded
{

	bool ToBool(float* f)
	{
		if(*f >= 0.0001f)
			return true;
		else
			return false;
	}

	void AndNode::SetInput(uint8_t index, float* output)
	{
		switch(index) {
			case 0: in1 = output; break;
			case 1: in2 = output; break;
			case 2: in3 = output; break;
			case 3: in4 = output; break;
			case 4: in5 = output; break;
		}
	}

	float* AndNode::GetOutput(uint8_t index)
	{
		return index == 0 ? &out : nullptr;
	}

	void AndNode::Evaluate(float timestep)
	{
		out = 0.0f;

		// All inputs are empty
		if(!in1 && !in2 && !in3 && !in4 && !in5)
			return;

		if(in1 && !ToBool(in1)) return;
		if(in2 && !ToBool(in2)) return;
		if(in3 && !ToBool(in3)) return;
		if(in4 && !ToBool(in4)) return;
		if(in5 && !ToBool(in5)) return;

		out = 1.0f;
	}


	void OrNode::SetInput(uint8_t index, float* output)
	{
		switch(index) {
			case 0: in1 = output; break;
			case 1: in2 = output; break;
			case 2: in3 = output; break;
			case 3: in4 = output; break;
			case 4: in5 = output; break;
		}
	}

	float* OrNode::GetOutput(uint8_t index)
	{
		return index == 0 ? &out : nullptr;
	}

	void OrNode::Evaluate(float timestep)
	{
		out = 1.0f;

		if(in1 && ToBool(in1)) return;
		if(in2 && ToBool(in2)) return;
		if(in3 && ToBool(in3)) return;
		if(in4 && ToBool(in4)) return;
		if(in5 && ToBool(in5)) return;

		out = 0.0f;
	}

	void TwoStageNode::SetInput(uint8_t index, float* output)
	{
		switch(index) {
			case 0: stage1Trigger = output; break;
			case 1: stage2Trigger = output; break;
		}
	}

	float* TwoStageNode::GetOutput(uint8_t index)
	{
		return index == 0 ? &out : nullptr;
	}

	void TwoStageNode::Evaluate(float timestep)
	{
		if(stage2Trigger && ToBool(stage2Trigger)) {
			out = stage2Percent;
			return;
		} else if(stage1Trigger && ToBool(stage1Trigger)) {
			out = stage1Percent;
			return;
		}
		out = 0.0f;
	}

	void CurveNode::SetInput(uint8_t index, float* output)
	{
		if(index == 0) 
			in = output;
	}

	float* CurveNode::GetOutput(uint8_t index)
	{
		return index == 0 ? &out : nullptr;
	}

	void CurveNode::Evaluate(float timestep)
	{
		if(!in) {
			out = 0.0f;
			return;
		}

		if(!ToBool(in)) {
			timerCounter = 0.0f;
			out = 0.0f;
			return;
		}

		timerCounter += timestep;
		if(timerCounter > timeout) {
			if(repeat)
				timerCounter = 0.0f;
			else
				timerCounter = timeout;
		}
		float curveProgress = timerCounter / timeout;

		switch(curveType) {
			case CurveNode::Types::Toggle:
				if(curveProgress > 0.5f)
					out = 1.0f;
				else
					out = 0.0f;
				break;
			case CurveNode::Types::Linear:
				out = curveProgress;
				break;
			case CurveNode::Types::Exponential:
				out = curveProgress * curveProgress;
				break;
			case CurveNode::Types::Breathing:
				float gamma = 0.20f; // affects the width of peak (more or less darkness)
				float beta = 0.5f;
				out = (expf(-(powf((curveProgress - beta) / gamma, 2.0f)) / 2.0f)); //(exp ^ exp) / 2.0f;
				break;
		}
	}

	void CompareNode::SetInput(uint8_t index, float* output)
	{
		switch(index) {
			case 0: input1 = output; break;
			case 1: input2 = output; break;
		}
	}

	float* CompareNode::GetOutput(uint8_t index)
	{
		return index == 0 ? &out : nullptr;
	}

	void CompareNode::Evaluate(float timestep)
	{
		if(!input1 || !input2) {
			out = 0.0f;
			return;
		}
		out = 0.0f;
		switch(compareType) {
			case CompareNode::Types::Greater:
				if(*input1 > *input2)
					out = 1.0f;
				break;
			case CompareNode::Types::GreaterEqual:
				if(*input1 >= *input2)
					out = 1.0f;
				break;
			case CompareNode::Types::Less:
				if(*input1 < *input2)
					out = 1.0f;
				break;
			case CompareNode::Types::LessEqual:
				if(*input1 <= *input2)
					out = 1.0f;
				break;
		}
	}

	void OnOffNode::SetInput(uint8_t index, float* output)
	{
		switch(index) {
			case 0: on = output; break;
			case 1: off = output; break;
		}
	}

	float* OnOffNode::GetOutput(uint8_t index)
	{
		return index == 0 ? &out : nullptr;
	}

	void OnOffNode::Evaluate(float timestep)
	{
		if(off && ToBool(off)) {
			out = 0.0f;
			return;
		} else if(on && ToBool(on)) {
			out = 1.0f;
			return;
		}
		out = 0.0f;
	}

	void InvertNode::SetInput(uint8_t index, float* output)
	{
		if(index == 0)
			in = output;
	}

	float* InvertNode::GetOutput(uint8_t index)
	{
		return index == 0 ? &out : nullptr;
	}

	void InvertNode::Evaluate(float timestep)
	{
		if(in && !ToBool(in)) {
			out = 1.0f;
			return;
		}
		out = 0.0f;
	}

	void ToggleNode::SetInput(uint8_t index, float* output)
	{
		if(index == 0)
			in = output;
	}

	float* ToggleNode::GetOutput(uint8_t index)
	{
		return index == 0 ? &out : nullptr;
	}

	void ToggleNode::Evaluate(float timestep)
	{
		if(!in) {
			out = 0.0f;
			return;
		}
		if(ToBool(in)) {
			if(lastValue) {
				return;
			} else {
				lastValue = true;
				if(ToBool(&out))
					out = 0.0f;
				else
					out = 1.0f;
			}
		} else {
			lastValue = false;
		}
	}

	void DelayNode::SetInput(uint8_t index, float* output)
	{
		if(index == 0)
			in = output;
	}

	float* DelayNode::GetOutput(uint8_t index)
	{
		return index == 0 ? &out : nullptr;
	}

	void DelayNode::Evaluate(float timestep)
	{
		if(!in) {
			counter = 0.0f;
			out = 0.0f;
			return;
		}

		if(ToBool(in)) {
			counter += timestep;
			if(counter >= delayTime) {
				out = *in;
				counter = delayTime;
			}
		} else {
			counter = 0.0f;
			out = 0.0f;
		}
	}

	void PushButtonNode::SetInput(uint8_t index, float* output)
	{
		switch(index) {
			case 0: button = output; break;
			case 1: neutralSafety = output; break;
			case 2: engineRunning = output; break;
		}
	}

	float* PushButtonNode::GetOutput(uint8_t index)
	{
		switch(index) {
			case 0: return &ignitionOut;
			case 1: return &starterOut;
		}
		return nullptr;
	}

	void PushButtonNode::Evaluate(float timestep)
	{
		if(!button || !neutralSafety || !engineRunning) {
			ignitionOut = 0.0f;
			starterOut = 0.0f;
			return;
		}

		bool pushButton = ToBool(button);

		if(engineRunning && ToBool(&starterOut)) {
			starterOut = 0.0f;
			return;
		}

		if(lastButtonState == pushButton) {
			return;
		}
		lastButtonState = pushButton;

		if(pushButton && !ToBool(&ignitionOut)) {
			ignitionOut = 1.0f;
			starterOut = 0.0f;
			return;
		}

		if(pushButton && !engineRunning && !neutralSafety) {
			ignitionOut = 0.0f;
			starterOut = 0.0f;
			return;
		}

		if(pushButton && engineRunning) {
			ignitionOut = 0.0f;
			starterOut = 0.0f;
			return;
		}


		if(pushButton && !engineRunning && neutralSafety) {
			starterOut = 1.0f;
			return;
		}

		starterOut = 0.0f;
	}

	void MapValueNode::SetInput(uint8_t index, float* output)
	{
		if(index == 0)
			in = output;
	}

	float* MapValueNode::GetOutput(uint8_t index)
	{
		return index == 0 ? &out : nullptr;
	}

	void MapValueNode::Evaluate(float timestep)
	{
		if(!in) {
			out = 0.0f;
			return;
		}

		float x = fromMin;
		float y = toMin;
		float x1 = fromMax;
		float y1 = toMax;
		float sloap = (y - y1) / (x - x1);

		out = (sloap * (*in - x1)) + y1;
	}

	void MathNode::SetInput(uint8_t index, float* output)
	{
		switch(index) {
			case 0: input1 = output; break;
			case 1: input2 = output; break;
		}
	}

	float* MathNode::GetOutput(uint8_t index)
	{
		return index == 0 ? &out : nullptr;
	}

	void MathNode::Evaluate(float timestep)
	{
		if(!input1 || !input2) {
			out = 0.0f;
			return;
		}

		switch(mathType) {
			case MathNode::Types::Add:
				out = *input1 + *input2;
				break;
			case MathNode::Types::Subtract:
				out = *input1 - *input2;
				break;
			case MathNode::Types::Multiply:
				out = *input1 * *input2;
				break;
			case MathNode::Types::Divide:
				out = *input1 / *input2;
				break;
		}
	}

	void ValueNode::SetInput(uint8_t index, float* output)
	{
		// None
	}

	float* ValueNode::GetOutput(uint8_t index)
	{
		return index == 0 ? &out : nullptr;
	}

	void ValueNode::Evaluate(float timestep)
	{
		// Nothing to do because its a static value
	}

	void SelectNode::SetInput(uint8_t index, float* output)
	{
		if(index == 0)
			selection = output;
	}

	float* SelectNode::GetOutput(uint8_t index)
	{
		return index == 0 ? &out : nullptr;
	}

	void SelectNode::Evaluate(float timestep)
	{
		if(!selection || !input1 || !input2) {
			out = 0.0f;
			return;
		}

		if(ToBool(selection))
			out = *input1;
		else
			out = *input2;
	}

	void InitalValueNode::SetInput(uint8_t index, float* output)
	{
		if(index == 0)
			rawData = output;
	}

	float* InitalValueNode::GetOutput(uint8_t index)
	{
		return index == 0 ? &out : nullptr;
	}

	void InitalValueNode::Evaluate(float timestep)
	{
		if(rawData)
			out = *rawData;
		else
			out = 0.0f;
	}

	void FinalValueNode::SetInput(uint8_t index, float* output)
	{
		if(index == 0)
			in = output;
	}

	float* FinalValueNode::GetOutput(uint8_t index)
	{
		// No outputs
		return nullptr;
	}

	void FinalValueNode::Evaluate(float timestep)
	{
		// Nothing to do
	}

	void NodeGroupNode::SetInput(uint8_t index, float* output)
	{
		if(index == 0)
			rawData = output;
	}

	float* NodeGroupNode::GetOutput(uint8_t index)
	{
		return index == 0 ? &out : nullptr;
	}

	void NodeGroupNode::Evaluate(float timestep)
	{
		if(rawData)
			out = *rawData;
		else
			out = 0.0f;
	}

}