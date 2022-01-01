#include "NodeVariant.h"

#include <new>

namespace Embedded
{
	NodeVariant::NodeVariant(NodeTypes type, float* ptr)
		: m_type(type), m_data(ptr)
	{
	}

	void NodeVariant::initalizeData()
	{
		if(!m_data)
			return;

		switch(m_type) {
			case Embedded::NodeTypes::Initial_Value:	new(m_data) InitalValueNode();		break;
			case Embedded::NodeTypes::Final_Value:		new(m_data) FinalValueNode();		break;
			case Embedded::NodeTypes::Node_Group:		new(m_data) NodeGroupNode();		break;
			case Embedded::NodeTypes::And:				new(m_data) AndNode();				break;
			case Embedded::NodeTypes::Or:				new(m_data) OrNode();				break;
			case Embedded::NodeTypes::Two_Stage:		new(m_data) TwoStageNode();			break;
			case Embedded::NodeTypes::Curve:			new(m_data) CurveNode();			break;
			case Embedded::NodeTypes::Compare:			new(m_data) CompareNode();			break;
			case Embedded::NodeTypes::On_Off:			new(m_data) OnOffNode();			break;
			case Embedded::NodeTypes::Invert:			new(m_data) InvertNode();			break;
			case Embedded::NodeTypes::Toggle:			new(m_data) ToggleNode();			break;
			case Embedded::NodeTypes::Delay:			new(m_data) DelayNode();			break;
			case Embedded::NodeTypes::Push_Button:		new(m_data) PushButtonNode();		break;
			case Embedded::NodeTypes::Map_Value:		new(m_data) MapValueNode();			break;
			case Embedded::NodeTypes::Math:				new(m_data) MathNode();				break;
			case Embedded::NodeTypes::Value:			new(m_data) ValueNode();			break;
			case Embedded::NodeTypes::Switch:			new(m_data) SwitchNode();			break;
			case Embedded::NodeTypes::Count:												break;
			default:																		break;
		}
	}

	uint32_t NodeVariant::getSize()
	{
		switch(m_type) {
			case Embedded::NodeTypes::Initial_Value:	return sizeof(InitalValueNode);
			case Embedded::NodeTypes::Final_Value:		return sizeof(FinalValueNode);
			case Embedded::NodeTypes::Node_Group:		return sizeof(NodeGroupNode);
			case Embedded::NodeTypes::And:				return sizeof(AndNode);
			case Embedded::NodeTypes::Or:				return sizeof(OrNode);
			case Embedded::NodeTypes::Two_Stage:		return sizeof(TwoStageNode);
			case Embedded::NodeTypes::Curve:			return sizeof(CurveNode);
			case Embedded::NodeTypes::Compare:			return sizeof(CompareNode);
			case Embedded::NodeTypes::On_Off:			return sizeof(OnOffNode);
			case Embedded::NodeTypes::Invert:			return sizeof(InvertNode);
			case Embedded::NodeTypes::Toggle:			return sizeof(ToggleNode);
			case Embedded::NodeTypes::Delay:			return sizeof(DelayNode);
			case Embedded::NodeTypes::Push_Button:		return sizeof(PushButtonNode);
			case Embedded::NodeTypes::Map_Value:		return sizeof(MapValueNode);
			case Embedded::NodeTypes::Math:				return sizeof(MathNode);
			case Embedded::NodeTypes::Value:			return sizeof(float);
			case Embedded::NodeTypes::Switch:			return sizeof(SwitchNode);
			case Embedded::NodeTypes::Count:			return 0;
			default:									return 0;
		}
	}

	void NodeVariant::setInput(uint8_t index, NodeInputMask& mask, float* output)
	{
		if(!m_data)
			return;

		switch(m_type) {
			case Embedded::NodeTypes::Initial_Value:	((InitalValueNode*) m_data)->SetInput(index, mask, output);	break;
			case Embedded::NodeTypes::Final_Value:		((FinalValueNode*) m_data)->SetInput(index, mask, output);	break;
			case Embedded::NodeTypes::Node_Group:		((NodeGroupNode*) m_data)->SetInput(index, mask, output);	break;
			case Embedded::NodeTypes::And:				((AndNode*) m_data)->SetInput(index, mask, output);			break;
			case Embedded::NodeTypes::Or:				((OrNode*) m_data)->SetInput(index, mask, output);			break;
			case Embedded::NodeTypes::Two_Stage:		((TwoStageNode*) m_data)->SetInput(index, mask, output);	break;
			case Embedded::NodeTypes::Curve:			((CurveNode*) m_data)->SetInput(index, mask, output);		break;
			case Embedded::NodeTypes::Compare:			((CompareNode*) m_data)->SetInput(index, mask, output);		break;
			case Embedded::NodeTypes::On_Off:			((OnOffNode*) m_data)->SetInput(index, mask, output);		break;
			case Embedded::NodeTypes::Invert:			((InvertNode*) m_data)->SetInput(index, mask, output);		break;
			case Embedded::NodeTypes::Toggle:			((ToggleNode*) m_data)->SetInput(index, mask, output);		break;
			case Embedded::NodeTypes::Delay:			((DelayNode*) m_data)->SetInput(index, mask, output);		break;
			case Embedded::NodeTypes::Push_Button:		((PushButtonNode*) m_data)->SetInput(index, mask, output);	break;
			case Embedded::NodeTypes::Map_Value:		((MapValueNode*) m_data)->SetInput(index, mask, output);	break;
			case Embedded::NodeTypes::Math:				((MathNode*) m_data)->SetInput(index, mask, output);		break;
			case Embedded::NodeTypes::Value:			((ValueNode*) m_data)->SetInput(index, mask, output);		break;
			case Embedded::NodeTypes::Switch:			((SwitchNode*) m_data)->SetInput(index, mask, output);		break;
			case Embedded::NodeTypes::Count:																		break;
			default:																								break;
		}
	}
	void NodeVariant::setValue(uint8_t index, NodeInputMask& mask, float value)
	{
		if(!m_data)
			return;

		switch(m_type) {
			case Embedded::NodeTypes::Initial_Value:	((InitalValueNode*) m_data)->SetValue(index, mask, value);	break;
			case Embedded::NodeTypes::Final_Value:		((FinalValueNode*) m_data)->SetValue(index, mask, value);	break;
			case Embedded::NodeTypes::Node_Group:		((NodeGroupNode*) m_data)->SetValue(index, mask, value);	break;
			case Embedded::NodeTypes::And:				((AndNode*) m_data)->SetValue(index, mask, value);			break;
			case Embedded::NodeTypes::Or:				((OrNode*) m_data)->SetValue(index, mask, value);			break;
			case Embedded::NodeTypes::Two_Stage:		((TwoStageNode*) m_data)->SetValue(index, mask, value);		break;
			case Embedded::NodeTypes::Curve:			((CurveNode*) m_data)->SetValue(index, mask, value);		break;
			case Embedded::NodeTypes::Compare:			((CompareNode*) m_data)->SetValue(index, mask, value);		break;
			case Embedded::NodeTypes::On_Off:			((OnOffNode*) m_data)->SetValue(index, mask, value);		break;
			case Embedded::NodeTypes::Invert:			((InvertNode*) m_data)->SetValue(index, mask, value);		break;
			case Embedded::NodeTypes::Toggle:			((ToggleNode*) m_data)->SetValue(index, mask, value);		break;
			case Embedded::NodeTypes::Delay:			((DelayNode*) m_data)->SetValue(index, mask, value);		break;
			case Embedded::NodeTypes::Push_Button:		((PushButtonNode*) m_data)->SetValue(index, mask, value);	break;
			case Embedded::NodeTypes::Map_Value:		((MapValueNode*) m_data)->SetValue(index, mask, value);		break;
			case Embedded::NodeTypes::Math:				((MathNode*) m_data)->SetValue(index, mask, value);			break;
			case Embedded::NodeTypes::Value:			((ValueNode*) m_data)->SetValue(index, mask, value);		break;
			case Embedded::NodeTypes::Switch:			((SwitchNode*) m_data)->SetValue(index, mask, value);		break;
			case Embedded::NodeTypes::Count:																		break;
			default:																								break;
		}
	}

	float* NodeVariant::getOutput(uint8_t index)
	{
		if(!m_data)
			return nullptr;

		switch(m_type) {
			case Embedded::NodeTypes::Initial_Value:	return ((InitalValueNode*) m_data)->GetOutput(index);
			case Embedded::NodeTypes::Final_Value:		return ((FinalValueNode*) m_data)->GetOutput(index);
			case Embedded::NodeTypes::Node_Group:		return ((NodeGroupNode*) m_data)->GetOutput(index);
			case Embedded::NodeTypes::And:				return ((AndNode*) m_data)->GetOutput(index);
			case Embedded::NodeTypes::Or:				return ((OrNode*) m_data)->GetOutput(index);
			case Embedded::NodeTypes::Two_Stage:		return ((TwoStageNode*) m_data)->GetOutput(index);
			case Embedded::NodeTypes::Curve:			return ((CurveNode*) m_data)->GetOutput(index);
			case Embedded::NodeTypes::Compare:			return ((CompareNode*) m_data)->GetOutput(index);
			case Embedded::NodeTypes::On_Off:			return ((OnOffNode*) m_data)->GetOutput(index);
			case Embedded::NodeTypes::Invert:			return ((InvertNode*) m_data)->GetOutput(index);
			case Embedded::NodeTypes::Toggle:			return ((ToggleNode*) m_data)->GetOutput(index);
			case Embedded::NodeTypes::Delay:			return ((DelayNode*) m_data)->GetOutput(index);
			case Embedded::NodeTypes::Push_Button:		return ((PushButtonNode*) m_data)->GetOutput(index);
			case Embedded::NodeTypes::Map_Value:		return ((MapValueNode*) m_data)->GetOutput(index);
			case Embedded::NodeTypes::Math:				return ((MathNode*) m_data)->GetOutput(index);
			case Embedded::NodeTypes::Value:			return ((ValueNode*) m_data)->GetOutput(index);
			case Embedded::NodeTypes::Switch:			return ((SwitchNode*) m_data)->GetOutput(index);
			case Embedded::NodeTypes::Count:			return nullptr;
			default:									return nullptr;
		}
	}

	void NodeVariant::evaluate(NodeInputMask& mask, float timestep)
	{
		if(!m_data)
			return;

		switch(m_type) {
			case Embedded::NodeTypes::Initial_Value:	((InitalValueNode*) m_data)->Evaluate(mask, timestep);	break;
			case Embedded::NodeTypes::Final_Value:		((FinalValueNode*) m_data)->Evaluate(mask, timestep);	break;
			case Embedded::NodeTypes::Node_Group:		((NodeGroupNode*) m_data)->Evaluate(mask, timestep);	break;
			case Embedded::NodeTypes::And:				((AndNode*) m_data)->Evaluate(mask, timestep);			break;
			case Embedded::NodeTypes::Or:				((OrNode*) m_data)->Evaluate(mask, timestep);			break;
			case Embedded::NodeTypes::Two_Stage:		((TwoStageNode*) m_data)->Evaluate(mask, timestep);		break;
			case Embedded::NodeTypes::Curve:			((CurveNode*) m_data)->Evaluate(mask, timestep);		break;
			case Embedded::NodeTypes::Compare:			((CompareNode*) m_data)->Evaluate(mask, timestep);		break;
			case Embedded::NodeTypes::On_Off:			((OnOffNode*) m_data)->Evaluate(mask, timestep);		break;
			case Embedded::NodeTypes::Invert:			((InvertNode*) m_data)->Evaluate(mask, timestep);		break;
			case Embedded::NodeTypes::Toggle:			((ToggleNode*) m_data)->Evaluate(mask, timestep);		break;
			case Embedded::NodeTypes::Delay:			((DelayNode*) m_data)->Evaluate(mask, timestep);		break;
			case Embedded::NodeTypes::Push_Button:		((PushButtonNode*) m_data)->Evaluate(mask, timestep);	break;
			case Embedded::NodeTypes::Map_Value:		((MapValueNode*) m_data)->Evaluate(mask, timestep);		break;
			case Embedded::NodeTypes::Math:				((MathNode*) m_data)->Evaluate(mask, timestep);			break;
			case Embedded::NodeTypes::Value:			((ValueNode*) m_data)->Evaluate(mask, timestep);		break;
			case Embedded::NodeTypes::Switch:			((SwitchNode*) m_data)->Evaluate(mask, timestep);		break;
			case Embedded::NodeTypes::Count:																	break;
			default:																							break;
		}
	}

}