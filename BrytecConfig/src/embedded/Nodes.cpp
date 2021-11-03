#include "Nodes.h"

namespace Embedded
{

	bool ToBool(float* f)
	{
		if(*f >= 0.0001f)
			return true;
		else
			return false;
	}

	void Evaluate(AndNode& node, float timestep)
	{
		node.out = 0.0f;

		if(node.in1 && !ToBool(node.in1)) return;
		if(node.in2 && !ToBool(node.in2)) return;
		if(node.in3 && !ToBool(node.in3)) return;
		if(node.in4 && !ToBool(node.in4)) return;
		if(node.in5 && !ToBool(node.in5)) return;

		node.out = 1.0f;
	}

	void Evaluate(OrNode& node, float timestep)
	{
		node.out = 1.0f;

		if(node.in1 && ToBool(node.in1)) return;
		if(node.in2 && ToBool(node.in2)) return;
		if(node.in3 && ToBool(node.in3)) return;
		if(node.in4 && ToBool(node.in4)) return;
		if(node.in5 && ToBool(node.in5)) return;

		node.out = 0.0f;
	}

	void Evaluate(TwoStageNode& node, float timestep)
	{
		if(node.stage2Trigger && ToBool(node.stage2Trigger)) {
			node.out = node.stage2Percent;
			return;
		} else if(node.stage1Trigger && ToBool(node.stage1Trigger)) {
			node.out = node.stage1Percent;
			return;
		}
		node.out = 0.0f;
	}

	void Evaluate(CurveNode& node, float timestep)
	{
		node.out = 0.0f;
		// TODO
#if 0
		if(!node.in) {
			node.out = 0.0f;
			return;
		}

		switch(node.curveType) {
			case CurveNode::Types::Toggle:
				if(ToBool(node.in)) {
					node.timerCounter += timestep;
				} else {
					node.timerCounter = 0.0f;
					node.out = 0.0f;
					return;
				}
				if(node.timerCounter >= node.timeout)
					node.timerCounter = node.timeout;

				if(ToBool(node.in) && !node.lastIn) {
					node.out = 100.0f;
					break;
				}
				if(ToBool(node.in) && !node.repeat && !ToBool(&node.out)) {
					node.timerCounter = 0;
					break;
				}
				if(ToBool(node.in)) {
					if(node.timerCounter >= node.timeout) {
						if(!ToBool(&node.out))
							node.out = 100.0f;
						else
							node.out = 0.0f;
						node.timerCounter = 0;
					}
				}
				break;

			case CurveNode::Types::Linear:

				break;
			case CurveNode::Types::Exponential:

				break;
			case CurveNode::Types::Breathing:

				break;

		}

		if(ToBool(node.in))
			node.lastIn = ToBool(node.in);
#endif
	}

	void Evaluate(CompareNode& node, float timestep)
	{
		if(!node.input1 || !node.input2) {
			node.out = 0.0f;
			return;
		}
		node.out = 0.0f;
		switch(node.compareType) {
			case CompareNode::Types::Greater:
				if(*node.input1 > *node.input2)
					node.out = 1.0f;
				break;
			case CompareNode::Types::GreaterEqual:
				if(*node.input1 >= *node.input2)
					node.out = 1.0f;
				break;
			case CompareNode::Types::Less:
				if(*node.input1 < *node.input2)
					node.out = 1.0f;
				break;
			case CompareNode::Types::LessEqual:
				if(*node.input1 <= *node.input2)
					node.out = 1.0f;
				break;
		}
	}

	void Evaluate(OnOffNode& node, float timestep)
	{
		if(node.off && ToBool(node.off)) {
			node.out = 0.0f;
			return;
		} else if(node.on && ToBool(node.on)) {
			node.out = 1.0f;
			return;
		}
		node.out = 0.0f;
	}

	void Evaluate(InvertNode& node, float timestep)
	{
		if(node.in && !ToBool(node.in)) {
			node.out = 1.0f;
			return;
		}
		node.out = 0.0f;
	}

	void Evaluate(ToggleNode& node, float timestep)
	{
		if(!node.in) {
			node.out = 0.0f;
			return;
		}
		if(ToBool(node.in)) {
			if(node.lastValue) {
				return;
			} else {
				node.lastValue = true;
				if(ToBool(&node.out))
					node.out = 0.0f;
				else
					node.out = 1.0f;
			}
		} else {
			node.lastValue = false;
		}
	}

	void Evaluate(DelayNode& node, float timestep)
	{
		if(!node.in) {
			node.counter = 0.0f;
			node.out = 0.0f;
			return;
		}

		if(ToBool(node.in)) {
			node.counter += timestep;
			if(node.counter >= node.delayTime) {
				node.out = *node.in;
				node.counter = node.delayTime;
			}
		} else {
			node.counter = 0.0f;
			node.out = 0.0f;
		}

	}

	void Evaluate(PushButtonNode& node, float timestep)
	{
		if(!node.button || !node.neutralSafety || !node.engineRunning) {
			node.ignitionOut = 0.0f;
			node.starterOut = 0.0f;
			return;
		}

		bool pushButton = ToBool(node.button);
		bool engineRunning = ToBool(node.engineRunning);
		bool neutralSafety = ToBool(node.neutralSafety);

		if(engineRunning && ToBool(&node.starterOut)) {
			node.starterOut = 0.0f;
			return;
		}

		if(node.lastButtonState == pushButton) {
			return;
		}
		node.lastButtonState = pushButton;

		if(pushButton && !ToBool(&node.ignitionOut)) {
			node.ignitionOut = 1.0f;
			node.starterOut = 0.0f;
			return;
		}

		if(pushButton && !engineRunning && !neutralSafety) {
			node.ignitionOut = 0.0f;
			node.starterOut = 0.0f;
			return;
		}

		if(pushButton && engineRunning) {
			node.ignitionOut = 0.0f;
			node.starterOut = 0.0f;
			return;
		}


		if(pushButton && !engineRunning && neutralSafety) {
			node.starterOut = 1.0f;
			return;
		}

		node.starterOut = 0.0f;
	}

	void Evaluate(MapValueNode& node, float timestep)
	{
		if(!node.in) {
			node.out = 0.0f;
			return;
		}

		float x = node.fromMin;
		float y = node.fromMax;
		float x1 = node.toMin;
		float y1 = node.toMax;
		float sloap = (y - y1) / (x - x1);

		node.out = (sloap * (*node.in - x1)) + y1;
	}

	void Evaluate(MathNode& node, float timestep)
	{
		if(!node.input1 || !node.input2) {
			node.out = 0.0f;
			return;
		}

		switch(node.mathType) {
			case MathNode::Types::Add:
				node.out = *node.input1 + *node.input2;
				break;
			case MathNode::Types::Subtract:
				node.out = *node.input1 - *node.input2;
				break;
			case MathNode::Types::Multiply:
				node.out = *node.input1 * *node.input2;
				break;
			case MathNode::Types::Divide:
				node.out = *node.input1 / *node.input2;
				break;
		}
	}

	void Evaluate(ValueNode& node, float timestep)
	{
		// Nothing to do because its a static value
	}

}