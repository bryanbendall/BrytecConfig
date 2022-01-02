#include "Nodes.h"

#include <math.h>

namespace Embedded
{

	bool ToBool(float& f)
	{
		if(f >= 0.0001f)
			return true;
		else
			return false;
	}

	void InitalValueNode::SetInput(uint8_t index, NodeInputMask& mask, float* output)
	{
		if(index == 0)
			m_rawData.pointer = output; mask.setPointer(index);
	}

	void InitalValueNode::SetValue(uint8_t index, NodeInputMask& mask, float value)
	{
		if(index == 0)
			m_rawData.value = value; mask.setValue(index);
	}

	float* InitalValueNode::GetOutput(uint8_t index)
	{
		return index == 0 ? &m_out : nullptr;
	}

	void InitalValueNode::Evaluate(NodeInputMask& mask, float timestep)
	{
		float rawData = mask.isPointer(0) ? *m_rawData.pointer : m_rawData.value;

		m_out = rawData;
	}

	void FinalValueNode::SetInput(uint8_t index, NodeInputMask& mask, float* output)
	{
		if(index == 0)
			m_in.pointer = output; mask.setPointer(index);
	}

	void FinalValueNode::SetValue(uint8_t index, NodeInputMask& mask, float value)
	{
		if(index == 0)
			m_in.value = value; mask.setValue(index);
	}

	float* FinalValueNode::GetOutput(uint8_t index)
	{
		// No outputs
		return nullptr;
	}

	void FinalValueNode::Evaluate(NodeInputMask& mask, float timestep)
	{
		// Nothing to do
	}

	void AndNode::SetInput(uint8_t index, NodeInputMask& mask, float* output)
	{
		switch(index) {
			case 0: m_in1.pointer = output; mask.setPointer(index); break;
			case 1: m_in2.pointer = output; mask.setPointer(index); break;
			case 2: m_in3.pointer = output; mask.setPointer(index); break;
			case 3: m_in4.pointer = output; mask.setPointer(index); break;
			case 4: m_in5.pointer = output; mask.setPointer(index); break;
		}
	}

	void AndNode::SetValue(uint8_t index, NodeInputMask& mask, float value)
	{
		switch(index) {
			case 0: m_in1.value = value; mask.setValue(index); break;
			case 1: m_in2.value = value; mask.setValue(index); break;
			case 2: m_in3.value = value; mask.setValue(index); break;
			case 3: m_in4.value = value; mask.setValue(index); break;
			case 4: m_in5.value = value; mask.setValue(index); break;
		}
	}

	float* AndNode::GetOutput(uint8_t index)
	{
		return index == 0 ? &m_out : nullptr;
	}

	void AndNode::Evaluate(NodeInputMask& mask, float timestep)
	{
		bool in1 = ToBool(mask.isPointer(0) ? *m_in1.pointer : m_in1.value);
		bool in2 = ToBool(mask.isPointer(1) ? *m_in2.pointer : m_in2.value);
		bool in3 = ToBool(mask.isPointer(2) ? *m_in3.pointer : m_in3.value);
		bool in4 = ToBool(mask.isPointer(3) ? *m_in4.pointer : m_in4.value);
		bool in5 = ToBool(mask.isPointer(4) ? *m_in5.pointer : m_in5.value);

		m_out = 0.0f;

		if(!in1) return;
		if(!in2) return;
		if(!in3) return;
		if(!in4) return;
		if(!in5) return;

		m_out = 1.0f;
	}


	void OrNode::SetInput(uint8_t index, NodeInputMask& mask, float* output)
	{
		switch(index) {
			case 0: m_in1.pointer = output; mask.setPointer(index); break;
			case 1: m_in2.pointer = output; mask.setPointer(index); break;
			case 2: m_in3.pointer = output; mask.setPointer(index); break;
			case 3: m_in4.pointer = output; mask.setPointer(index); break;
			case 4: m_in5.pointer = output; mask.setPointer(index); break;
		}
	}

	void OrNode::SetValue(uint8_t index, NodeInputMask& mask, float value)
	{
		switch(index) {
			case 0: m_in1.value = value; mask.setValue(index); break;
			case 1: m_in2.value = value; mask.setValue(index); break;
			case 2: m_in3.value = value; mask.setValue(index); break;
			case 3: m_in4.value = value; mask.setValue(index); break;
			case 4: m_in5.value = value; mask.setValue(index); break;
		}
	}

	float* OrNode::GetOutput(uint8_t index)
	{
		return index == 0 ? &m_out : nullptr;
	}

	void OrNode::Evaluate(NodeInputMask& mask, float timestep)
	{
		bool in1 = ToBool(mask.isPointer(0) ? *m_in1.pointer : m_in1.value);
		bool in2 = ToBool(mask.isPointer(1) ? *m_in2.pointer : m_in2.value);
		bool in3 = ToBool(mask.isPointer(2) ? *m_in3.pointer : m_in3.value);
		bool in4 = ToBool(mask.isPointer(3) ? *m_in4.pointer : m_in4.value);
		bool in5 = ToBool(mask.isPointer(4) ? *m_in5.pointer : m_in5.value);
		
		m_out = 1.0f;

		if(in1) return;
		if(in2) return;
		if(in3) return;
		if(in4) return;
		if(in5) return;

		m_out = 0.0f;
	}

	void TwoStageNode::SetInput(uint8_t index, NodeInputMask& mask, float* output)
	{
		switch(index) {
			case 0: m_stage1Trigger.pointer = output; mask.setPointer(index); break;
			case 1: m_stage1Percent.pointer = output; mask.setPointer(index); break;
			case 2: m_stage2Trigger.pointer = output; mask.setPointer(index); break;
			case 3: m_stage2Percent.pointer = output; mask.setPointer(index); break;
		}
	}

	void TwoStageNode::SetValue(uint8_t index, NodeInputMask& mask, float value)
	{
		switch(index) {
			case 0: m_stage1Trigger.value = value; mask.setValue(index); break;
			case 1: m_stage1Percent.value = value; mask.setValue(index); break;
			case 2: m_stage2Trigger.value = value; mask.setValue(index); break;
			case 3: m_stage2Percent.value = value; mask.setValue(index); break;
		}
	}

	float* TwoStageNode::GetOutput(uint8_t index)
	{
		return index == 0 ? &m_out : nullptr;
	}

	void TwoStageNode::Evaluate(NodeInputMask& mask, float timestep)
	{
		bool stage1Trigger = ToBool(mask.isPointer(0) ? *m_stage1Trigger.pointer : m_stage1Trigger.value);
		float& stage1Percent = mask.isPointer(1) ? *m_stage1Percent.pointer : m_stage1Percent.value;
		bool stage2Trigger = ToBool(mask.isPointer(2) ? *m_stage2Trigger.pointer : m_stage2Trigger.value);
		float& stage2Percent = mask.isPointer(3) ? *m_stage2Percent.pointer : m_stage2Percent.value;

		if(stage2Trigger) {
			m_out = stage2Percent;
			return;
		} else if(stage1Trigger) {
			m_out = stage1Percent;
			return;
		}
		m_out = 0.0f;
	}

	void CurveNode::SetInput(uint8_t index, NodeInputMask& mask, float* output)
	{
		switch(index) {
			case 0: m_in.pointer = output; mask.setPointer(index); break;
			case 1: m_repeat.pointer = output; mask.setPointer(index); break;
			case 2: m_timeout.pointer = output; mask.setPointer(index); break;
		}
	}

	void CurveNode::SetValue(uint8_t index, NodeInputMask& mask, float value)
	{
		switch(index) {
			case 0: m_in.value = value; mask.setValue(index); break;
			case 1: m_repeat.value = value; mask.setValue(index); break;
			case 2: m_timeout.value = value; mask.setValue(index); break;
			case 3: m_curveType.value = value; mask.setValue(index); break;
			case 4: m_timerCounter.value = value; mask.setValue(index); break;
		}
	}

	float* CurveNode::GetOutput(uint8_t index)
	{
		return index == 0 ? &m_out : nullptr;
	}

	void CurveNode::Evaluate(NodeInputMask& mask, float timestep)
	{		 
		bool in = ToBool(mask.isPointer(0) ? *m_in.pointer : m_in.value);
		bool repeat = ToBool(mask.isPointer(1) ? *m_repeat.pointer : m_repeat.value);
		float& timeout = mask.isPointer(2) ? *m_timeout.pointer : m_timeout.value;
		Types curveType = (Types)(mask.isPointer(3) ? *m_curveType.pointer : m_curveType.value);
		float& timerCounter = mask.isPointer(4) ? *m_timerCounter.pointer : m_timerCounter.value;

		if(!in) {
			timerCounter = 0.0f;
			m_out = 0.0f;
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
					m_out = 1.0f;
				else
					m_out = 0.0f;
				break;
			case CurveNode::Types::Linear:
				m_out = curveProgress;
				break;
			case CurveNode::Types::Exponential:
				m_out = curveProgress * curveProgress;
				break;
			case CurveNode::Types::Breathing:
				float gamma = 0.20f; // affects the width of peak (more or less darkness)
				float beta = 0.5f;
				m_out = (expf(-(powf((curveProgress - beta) / gamma, 2.0f)) / 2.0f)); //(exp ^ exp) / 2.0f;
				break;
		}
	}

	void CompareNode::SetInput(uint8_t index, NodeInputMask& mask, float* output)
	{
		switch(index) {
			case 0: m_input1.pointer = output; mask.setPointer(index); break;
			case 1: m_input2.pointer = output; mask.setPointer(index); break;
		}
	}

	void CompareNode::SetValue(uint8_t index, NodeInputMask& mask, float value)
	{
		switch(index) {
			case 0: m_input1.value = value; mask.setValue(index); break;
			case 1: m_input2.value = value; mask.setValue(index); break;
			case 2: m_compareType.value = value; mask.setValue(index); break;
		}
	}

	float* CompareNode::GetOutput(uint8_t index)
	{
		return index == 0 ? &m_out : nullptr;
	}

	void CompareNode::Evaluate(NodeInputMask& mask, float timestep)
	{
		float& input1 = mask.isPointer(0) ? *m_input1.pointer : m_input1.value;
		float& input2= mask.isPointer(1) ? *m_input2.pointer : m_input2.value;
		Types compareType = (Types)(mask.isPointer(2) ? *m_compareType.pointer : m_compareType.value);

		m_out = 0.0f;
		switch(compareType) {
			case CompareNode::Types::Greater:
				if(input1 > input2)
					m_out = 1.0f;
				break;
			case CompareNode::Types::GreaterEqual:
				if(input1 >= input2)
					m_out = 1.0f;
				break;
			case CompareNode::Types::Less:
				if(input1 < input2)
					m_out = 1.0f;
				break;
			case CompareNode::Types::LessEqual:
				if(input1 <= input2)
					m_out = 1.0f;
				break;
		}
	}

	void OnOffNode::SetInput(uint8_t index, NodeInputMask& mask, float* output)
	{
		switch(index) {
			case 0: m_on.pointer = output; mask.setPointer(index); break;
			case 1: m_off.pointer = output; mask.setPointer(index); break;
		}
	}

	void OnOffNode::SetValue(uint8_t index, NodeInputMask& mask, float value)
	{
		switch(index) {
			case 0: m_on.value = value; mask.setValue(index); break;
			case 1: m_off.value = value; mask.setValue(index); break;
		}
	}

	float* OnOffNode::GetOutput(uint8_t index)
	{
		return index == 0 ? &m_out : nullptr;
	}

	void OnOffNode::Evaluate(NodeInputMask& mask, float timestep)
	{
		bool on = ToBool(mask.isPointer(0) ? *m_on.pointer : m_on.value);
		bool off = ToBool(mask.isPointer(1) ? *m_off.pointer : m_off.value);

		if(off) {
			m_out = 0.0f;
			return;
		} else if(on) {
			m_out = 1.0f;
			return;
		}
		m_out = 0.0f;
	}

	void InvertNode::SetInput(uint8_t index, NodeInputMask& mask, float* output)
	{
		switch(index) {
			case 0: m_in.pointer = output; mask.setPointer(index); break;
		}
	}

	void InvertNode::SetValue(uint8_t index, NodeInputMask& mask, float value)
	{
		switch(index) {
			case 0: m_in.value = value; mask.setValue(index); break;
		}
	}

	float* InvertNode::GetOutput(uint8_t index)
	{
		return index == 0 ? &m_out : nullptr;
	}

	void InvertNode::Evaluate(NodeInputMask& mask, float timestep)
	{
		bool in = ToBool(mask.isPointer(0) ? *m_in.pointer : m_in.value);

		if(!in) {
			m_out = 1.0f;
			return;
		}
		m_out = 0.0f;
	}

	void ToggleNode::SetInput(uint8_t index, NodeInputMask& mask, float* output)
	{
		switch(index) {
			case 0: m_in.pointer = output; mask.setPointer(index); break;
		}
	}

	void ToggleNode::SetValue(uint8_t index, NodeInputMask& mask, float value)
	{
		switch(index) {
			case 0: m_in.value = value; mask.setValue(index); break;
			case 1: m_lastValue.value = value; mask.setValue(index); break;
		}
	}

	float* ToggleNode::GetOutput(uint8_t index)
	{
		return index == 0 ? &m_out : nullptr;
	}

	void ToggleNode::Evaluate(NodeInputMask& mask, float timestep)
	{
		bool in = ToBool(mask.isPointer(0) ? *m_in.pointer : m_in.value);
		float& lastValue = mask.isPointer(1) ? *m_lastValue.pointer : m_lastValue.value;

		if(in) {
			if(ToBool(lastValue)) {
				return;
			} else {
				lastValue = true;
				if(ToBool(m_out))
					m_out = 0.0f;
				else
					m_out = 1.0f;
			}
		} else {
			lastValue = false;
		}
	}

	void DelayNode::SetInput(uint8_t index, NodeInputMask& mask, float* output)
	{
		switch(index) {
			case 0: m_in.pointer = output; mask.setPointer(index); break;
			case 1: m_delayTime.pointer = output; mask.setPointer(index); break;
		}
	}

	void DelayNode::SetValue(uint8_t index, NodeInputMask& mask, float value)
	{
		switch(index) {
			case 0: m_in.value = value; mask.setValue(index); break;
			case 1: m_delayTime.value = value; mask.setValue(index); break;
			case 2: m_counter.value = value; mask.setValue(index); break;
		}
	}

	float* DelayNode::GetOutput(uint8_t index)
	{
		return index == 0 ? &m_out : nullptr;
	}

	void DelayNode::Evaluate(NodeInputMask& mask, float timestep)
	{
		bool in = ToBool(mask.isPointer(0) ? *m_in.pointer : m_in.value);
		float& delayTime = mask.isPointer(1) ? *m_delayTime.pointer : m_delayTime.value;
		float& counter = mask.isPointer(2) ? *m_counter.pointer : m_counter.value;

		if(in) {
			counter += timestep;
			if(counter >= delayTime) {
				m_out = in;
				counter = delayTime;
			}
		} else {
			counter = 0.0f;
			m_out = 0.0f;
		}
	}

	void PushButtonNode::SetInput(uint8_t index, NodeInputMask& mask, float* output)
	{
		switch(index) {
			case 0: m_button.pointer = output; mask.setPointer(index); break;
			case 1: m_neutralSafety.pointer = output; mask.setPointer(index); break;
			case 2: m_engineRunning.pointer = output; mask.setPointer(index); break;
		}
	}

	void PushButtonNode::SetValue(uint8_t index, NodeInputMask& mask, float value)
	{
		switch(index) {
			case 0: m_button.value = value; mask.setValue(index); break;
			case 1: m_neutralSafety.value = value; mask.setValue(index); break;
			case 2: m_engineRunning.value = value; mask.setValue(index); break;
			case 3: m_lastButtonState.value = value; mask.setValue(index); break;
		}
	}

	float* PushButtonNode::GetOutput(uint8_t index)
	{
		switch(index) {
			case 0: return &m_ignitionOut;
			case 1: return &m_starterOut;
		}
		return nullptr;
	}

	void PushButtonNode::Evaluate(NodeInputMask& mask, float timestep)
	{
		bool button = ToBool(mask.isPointer(0) ? *m_button.pointer : m_button.value);
		bool neutralSafety = ToBool(mask.isPointer(1) ? *m_neutralSafety.pointer : m_neutralSafety.value);
		bool engineRunning = ToBool(mask.isPointer(2) ? *m_engineRunning.pointer : m_engineRunning.value);
		float& lastButtonState = mask.isPointer(3) ? *m_lastButtonState.pointer : m_lastButtonState.value;

		if(engineRunning && ToBool(m_starterOut)) {
			m_starterOut = 0.0f;
			return;
		}

		if(ToBool(lastButtonState) == button) {
			return;
		}
		lastButtonState = button;

		if(button && !ToBool(m_ignitionOut)) {
			m_ignitionOut = 1.0f;
			m_starterOut = 0.0f;
			return;
		}

		if(button && !engineRunning && !neutralSafety) {
			m_ignitionOut = 0.0f;
			m_starterOut = 0.0f;
			return;
		}

		if(button && engineRunning) {
			m_ignitionOut = 0.0f;
			m_starterOut = 0.0f;
			return;
		}


		if(button && !engineRunning && neutralSafety) {
			m_starterOut = 1.0f;
			return;
		}

		m_starterOut = 0.0f;
	}

	void MapValueNode::SetInput(uint8_t index, NodeInputMask& mask, float* output)
	{
		switch(index) {
			case 0: m_in.pointer = output; mask.setPointer(index); break;
			case 1: m_fromMin.pointer = output; mask.setPointer(index); break;
			case 2: m_fromMax.pointer = output; mask.setPointer(index); break;
			case 3: m_toMin.pointer = output; mask.setPointer(index); break;
			case 4: m_toMax.pointer = output; mask.setPointer(index); break;
		}
	}

	void MapValueNode::SetValue(uint8_t index, NodeInputMask& mask, float value)
	{
		switch(index) {
			case 0: m_in.value = value; mask.setValue(index); break;
			case 1: m_fromMin.value = value; mask.setValue(index); break;
			case 2: m_fromMax.value = value; mask.setValue(index); break;
			case 3: m_toMin.value = value; mask.setValue(index); break;
			case 4: m_toMax.value = value; mask.setValue(index); break;
		}
	}

	float* MapValueNode::GetOutput(uint8_t index)
	{
		return index == 0 ? &m_out : nullptr;
	}

	void MapValueNode::Evaluate(NodeInputMask& mask, float timestep)
	{
		float& in = mask.isPointer(0) ? *m_in.pointer : m_in.value;
		float& fromMin = mask.isPointer(1) ? *m_fromMin.pointer : m_fromMin.value;
		float& fromMax = mask.isPointer(2) ? *m_fromMax.pointer : m_fromMax.value;
		float& toMin = mask.isPointer(3) ? *m_toMin.pointer : m_toMin.value;
		float& toMax = mask.isPointer(4) ? *m_toMax.pointer : m_toMax.value;

		float x = fromMin;
		float y = toMin;
		float x1 = fromMax;
		float y1 = toMax;
		float sloap = (y - y1) / (x - x1);

		m_out = (sloap * (in - x1)) + y1;
	}

	void MathNode::SetInput(uint8_t index, NodeInputMask& mask, float* output)
	{
		switch(index) {
			case 0: m_input1.pointer = output; mask.setPointer(index); break;
			case 1: m_input2.pointer = output; mask.setPointer(index); break;
		}
	}

	void MathNode::SetValue(uint8_t index, NodeInputMask& mask, float value)
	{
		switch(index) {
			case 0: m_input1.value = value; mask.setValue(index); break;
			case 1: m_input2.value = value; mask.setValue(index); break;
			case 2: m_mathType.value = value; mask.setValue(index); break;
		}
	}

	float* MathNode::GetOutput(uint8_t index)
	{
		return index == 0 ? &m_out : nullptr;
	}

	void MathNode::Evaluate(NodeInputMask& mask, float timestep)
	{
		float& input1 = mask.isPointer(0) ? *m_input1.pointer : m_input1.value;
		float& input2 = mask.isPointer(1) ? *m_input2.pointer : m_input2.value;
		Types mathType = (Types)(mask.isPointer(2) ? *m_mathType.pointer : m_mathType.value);

		switch(mathType) {
			case MathNode::Types::Add:
				m_out = input1 + input2;
				break;
			case MathNode::Types::Subtract:
				m_out = input1 - input2;
				break;
			case MathNode::Types::Multiply:
				m_out = input1 * input2;
				break;
			case MathNode::Types::Divide:
				m_out = input1 / input2;
				break;
		}
	}

	void ValueNode::SetInput(uint8_t index, NodeInputMask& mask, float* output)
	{
		// None
	}

	void ValueNode::SetValue(uint8_t index, NodeInputMask& mask, float value)
	{
		// Special case because it is a static value
		if(index == 0)
			m_out = value;
	}

	float* ValueNode::GetOutput(uint8_t index)
	{
		return index == 0 ? &m_out : nullptr;
	}

	void ValueNode::Evaluate(NodeInputMask& mask, float timestep)
	{
		// Nothing to do because its a static value
	}

	void SwitchNode::SetInput(uint8_t index, NodeInputMask& mask, float* output)
	{
		switch(index) {
			case 0: m_selection.pointer = output; mask.setPointer(index); break;
			case 1: m_input1.pointer = output; mask.setPointer(index); break;
			case 2: m_input2.pointer = output; mask.setPointer(index); break;
		}
	}

	void SwitchNode::SetValue(uint8_t index, NodeInputMask& mask, float value)
	{
		switch(index) {
			case 0: m_selection.value = value; mask.setValue(index); break;
			case 1: m_input1.value = value; mask.setValue(index); break;
			case 2: m_input2.value = value; mask.setValue(index); break;
		}
	}

	float* SwitchNode::GetOutput(uint8_t index)
	{
		return index == 0 ? &m_out : nullptr;
	}

	void SwitchNode::Evaluate(NodeInputMask& mask, float timestep)
	{
		bool selection = ToBool(mask.isPointer(0) ? *m_selection.pointer : m_selection.value);
		float& input1 = mask.isPointer(1) ? *m_input1.pointer : m_input1.value;
		float& input2 = mask.isPointer(2) ? *m_input2.pointer : m_input2.value;

		if(selection)
			m_out = input1;
		else
			m_out = input2;
	}

	void NodeGroupNode::SetInput(uint8_t index, NodeInputMask& mask, float* output)
	{
		switch(index) {
			case 0: m_rawData.pointer = output; mask.setPointer(index); break;
		}
	}

	void NodeGroupNode::SetValue(uint8_t index, NodeInputMask& mask, float value)
	{
		switch(index) {
			case 0: m_rawData.value = value; mask.setValue(index); break;
		}
	}

	float* NodeGroupNode::GetOutput(uint8_t index)
	{
		return index == 0 ? &m_out : nullptr;
	}

	void NodeGroupNode::Evaluate(NodeInputMask& mask, float timestep)
	{
		float& rawData = mask.isPointer(0) ? *m_rawData.pointer : m_rawData.value;

		if(rawData)
			m_out = rawData;
		else
			m_out = 0.0f;
	}

}