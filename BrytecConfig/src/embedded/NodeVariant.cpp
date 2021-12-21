#include "NodeVariant.h"

#include <new>

namespace Embedded
{
	NodeVariant::NodeVariant(uint8_t* ptr)
		: pointer(ptr), type((NodeTypes) *ptr), data(++ptr)
	{
	}

	void NodeVariant::initalizeData()
	{
		switch(type) {
			case Embedded::NodeTypes::Initial_Value:	new(data) InitalValueNode();	break;
			case Embedded::NodeTypes::Final_Value:		new(data) FinalValueNode();		break;
			case Embedded::NodeTypes::Node_Group:		new(data) NodeGroupNode();		break;
			case Embedded::NodeTypes::And:				new(data) AndNode();			break;
			case Embedded::NodeTypes::Or:				new(data) OrNode();				break;
			case Embedded::NodeTypes::Two_Stage:		new(data) TwoStageNode();		break;
			case Embedded::NodeTypes::Curve:			new(data) CurveNode();			break;
			case Embedded::NodeTypes::Compare:			new(data) CompareNode();		break;
			case Embedded::NodeTypes::On_Off:			new(data) OnOffNode();			break;
			case Embedded::NodeTypes::Invert:			new(data) InvertNode();			break;
			case Embedded::NodeTypes::Toggle:			new(data) ToggleNode();			break;
			case Embedded::NodeTypes::Delay:			new(data) DelayNode();			break;
			case Embedded::NodeTypes::Push_Button:		new(data) PushButtonNode();		break;
			case Embedded::NodeTypes::Map_Value:		new(data) MapValueNode();		break;
			case Embedded::NodeTypes::Math:				new(data) MathNode();			break;
			case Embedded::NodeTypes::Value:			new(data) ValueNode();			break;
			case Embedded::NodeTypes::Switch:			new(data) SwitchNode();			break;
			case Embedded::NodeTypes::Count:											break;
			default:																	break;
		}
	}

	uint32_t NodeVariant::getSize()
	{
		switch(type) {
			case Embedded::NodeTypes::Initial_Value:	return 1 + sizeof(InitalValueNode);
			case Embedded::NodeTypes::Final_Value:		return 1 + sizeof(FinalValueNode);
			case Embedded::NodeTypes::Node_Group:		return 1 + sizeof(NodeGroupNode);
			case Embedded::NodeTypes::And:				return 1 + sizeof(AndNode);
			case Embedded::NodeTypes::Or:				return 1 + sizeof(OrNode);
			case Embedded::NodeTypes::Two_Stage:		return 1 + sizeof(TwoStageNode);
			case Embedded::NodeTypes::Curve:			return 1 + sizeof(CurveNode);
			case Embedded::NodeTypes::Compare:			return 1 + sizeof(CompareNode);
			case Embedded::NodeTypes::On_Off:			return 1 + sizeof(OnOffNode);
			case Embedded::NodeTypes::Invert:			return 1 + sizeof(InvertNode);
			case Embedded::NodeTypes::Toggle:			return 1 + sizeof(ToggleNode);
			case Embedded::NodeTypes::Delay:			return 1 + sizeof(DelayNode);
			case Embedded::NodeTypes::Push_Button:		return 1 + sizeof(PushButtonNode);
			case Embedded::NodeTypes::Map_Value:		return 1 + sizeof(MapValueNode);
			case Embedded::NodeTypes::Math:				return 1 + sizeof(MathNode);
			case Embedded::NodeTypes::Value:			return 1 + sizeof(float);
			case Embedded::NodeTypes::Switch:			return 1 + sizeof(SwitchNode);
			case Embedded::NodeTypes::Count:			return 0;
			default:									return 0;
		}
	}

	void NodeVariant::evaluate(float timestep)
	{
		switch(type) {
			case Embedded::NodeTypes::Initial_Value:	((InitalValueNode*) data)->Evaluate(timestep);	break;
			case Embedded::NodeTypes::Final_Value:		((FinalValueNode*) data)->Evaluate(timestep);	break;
			case Embedded::NodeTypes::Node_Group:		((NodeGroupNode*) data)->Evaluate(timestep);	break;
			case Embedded::NodeTypes::And:				((AndNode*) data)->Evaluate(timestep);			break;
			case Embedded::NodeTypes::Or:				((OrNode*) data)->Evaluate(timestep);			break;
			case Embedded::NodeTypes::Two_Stage:		((TwoStageNode*) data)->Evaluate(timestep);		break;
			case Embedded::NodeTypes::Curve:			((CurveNode*) data)->Evaluate(timestep);		break;
			case Embedded::NodeTypes::Compare:			((CompareNode*) data)->Evaluate(timestep);		break;
			case Embedded::NodeTypes::On_Off:			((OnOffNode*) data)->Evaluate(timestep);		break;
			case Embedded::NodeTypes::Invert:			((InvertNode*) data)->Evaluate(timestep);		break;
			case Embedded::NodeTypes::Toggle:			((ToggleNode*) data)->Evaluate(timestep);		break;
			case Embedded::NodeTypes::Delay:			((DelayNode*) data)->Evaluate(timestep);		break;
			case Embedded::NodeTypes::Push_Button:		((PushButtonNode*) data)->Evaluate(timestep);	break;
			case Embedded::NodeTypes::Map_Value:		((MapValueNode*) data)->Evaluate(timestep);		break;
			case Embedded::NodeTypes::Math:				((MathNode*) data)->Evaluate(timestep);			break;
			case Embedded::NodeTypes::Value:			((ValueNode*) data)->Evaluate(timestep);		break;
			case Embedded::NodeTypes::Switch:			((SwitchNode*) data)->Evaluate(timestep);		break;
			case Embedded::NodeTypes::Count:															break;
			default:																					break;
		}
	}

	float* NodeVariant::getOutput(uint8_t index)
	{
		switch(type) {
			case Embedded::NodeTypes::Initial_Value:	return ((InitalValueNode*) data)->GetOutput(index);
			case Embedded::NodeTypes::Final_Value:		return ((FinalValueNode*) data)->GetOutput(index);
			case Embedded::NodeTypes::Node_Group:		return ((NodeGroupNode*) data)->GetOutput(index);
			case Embedded::NodeTypes::And:				return ((AndNode*) data)->GetOutput(index);
			case Embedded::NodeTypes::Or:				return ((OrNode*) data)->GetOutput(index);
			case Embedded::NodeTypes::Two_Stage:		return ((TwoStageNode*) data)->GetOutput(index);
			case Embedded::NodeTypes::Curve:			return ((CurveNode*) data)->GetOutput(index);
			case Embedded::NodeTypes::Compare:			return ((CompareNode*) data)->GetOutput(index);
			case Embedded::NodeTypes::On_Off:			return ((OnOffNode*) data)->GetOutput(index);
			case Embedded::NodeTypes::Invert:			return ((InvertNode*) data)->GetOutput(index);
			case Embedded::NodeTypes::Toggle:			return ((ToggleNode*) data)->GetOutput(index);
			case Embedded::NodeTypes::Delay:			return ((DelayNode*) data)->GetOutput(index);
			case Embedded::NodeTypes::Push_Button:		return ((PushButtonNode*) data)->GetOutput(index);
			case Embedded::NodeTypes::Map_Value:		return ((MapValueNode*) data)->GetOutput(index);
			case Embedded::NodeTypes::Math:				return ((MathNode*) data)->GetOutput(index);
			case Embedded::NodeTypes::Value:			return ((ValueNode*) data)->GetOutput(index);
			case Embedded::NodeTypes::Switch:			return ((SwitchNode*) data)->GetOutput(index);
			case Embedded::NodeTypes::Count:			return nullptr;
			default:									return nullptr;
		}
	}

	void NodeVariant::setInput(uint8_t index, float* output)
	{
		switch(type) {
			case Embedded::NodeTypes::Initial_Value:	((InitalValueNode*) data)->SetInput(index, output);	break;
			case Embedded::NodeTypes::Final_Value:		((FinalValueNode*) data)->SetInput(index, output);	break;
			case Embedded::NodeTypes::Node_Group:		((NodeGroupNode*) data)->SetInput(index, output);	break;
			case Embedded::NodeTypes::And:				((AndNode*) data)->SetInput(index, output);			break;
			case Embedded::NodeTypes::Or:				((OrNode*) data)->SetInput(index, output);			break;
			case Embedded::NodeTypes::Two_Stage:		((TwoStageNode*) data)->SetInput(index, output);	break;
			case Embedded::NodeTypes::Curve:			((CurveNode*) data)->SetInput(index, output);		break;
			case Embedded::NodeTypes::Compare:			((CompareNode*) data)->SetInput(index, output);		break;
			case Embedded::NodeTypes::On_Off:			((OnOffNode*) data)->SetInput(index, output);		break;
			case Embedded::NodeTypes::Invert:			((InvertNode*) data)->SetInput(index, output);		break;
			case Embedded::NodeTypes::Toggle:			((ToggleNode*) data)->SetInput(index, output);		break;
			case Embedded::NodeTypes::Delay:			((DelayNode*) data)->SetInput(index, output);		break;
			case Embedded::NodeTypes::Push_Button:		((PushButtonNode*) data)->SetInput(index, output);	break;
			case Embedded::NodeTypes::Map_Value:		((MapValueNode*) data)->SetInput(index, output);	break;
			case Embedded::NodeTypes::Math:				((MathNode*) data)->SetInput(index, output);		break;
			case Embedded::NodeTypes::Value:			((ValueNode*) data)->SetInput(index, output);		break;
			case Embedded::NodeTypes::Switch:			((SwitchNode*) data)->SetInput(index, output);		break;
			case Embedded::NodeTypes::Count:																break;
			default:																						break;
		}
	}
}