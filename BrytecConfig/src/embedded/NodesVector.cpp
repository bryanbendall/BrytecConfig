#include "NodesVector.h"

#include <cstring>

namespace Embedded
{
	NodeVariant::NodeVariant(uint8_t* ptr)
		: pointer(ptr), type((NodeTypes) *ptr), data(++ptr)
	{
	}

	void NodeVariant::initalizeData()
	{
		switch(type) {
			//case Embedded::NodeTypes::Initial_Value:	Evaluate(float); //TODO
			//case Embedded::NodeTypes::Final_Value:		Evaluate(float); //TODO
			//case Embedded::NodeTypes::Node_Group:		Evaluate(*(AndNode*) data, 0.0f);			break;
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
				//case Embedded::NodeTypes::Value:			Evaluate(*(float);
			case Embedded::NodeTypes::Select:			new(data) SelectNode();			break;
				//case Embedded::NodeTypes::Count:			Evaluate
				//default:									Evaluate
		}
	}

	uint32_t NodeVariant::getSize()
	{
		switch(type) {
			case Embedded::NodeTypes::Initial_Value:	return 1 + sizeof(float); //TODO
			case Embedded::NodeTypes::Final_Value:		return 1 + sizeof(float); //TODO
			case Embedded::NodeTypes::Node_Group:		return 1 + sizeof(AndNode);
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
			case Embedded::NodeTypes::Select:			return 1 + sizeof(SelectNode);
			case Embedded::NodeTypes::Count:			return 0;
			default:									return 0;
		}
	}

	void NodeVariant::evaluate()
	{
		switch(type) {
			//case Embedded::NodeTypes::Initial_Value:	Evaluate(float); //TODO
			//case Embedded::NodeTypes::Final_Value:		Evaluate(float); //TODO
			//case Embedded::NodeTypes::Node_Group:		Evaluate(*(AndNode*) data, 0.0f);			break;
			case Embedded::NodeTypes::And:				Evaluate(*(AndNode*)data, 0.0f);			break;
			case Embedded::NodeTypes::Or:				Evaluate(*(OrNode*)data, 0.0f);				break;
			case Embedded::NodeTypes::Two_Stage:		Evaluate(*(TwoStageNode*)data, 0.0f);		break;
			case Embedded::NodeTypes::Curve:			Evaluate(*(CurveNode*)data, 0.0f);			break;
			case Embedded::NodeTypes::Compare:			Evaluate(*(CompareNode*)data, 0.0f);		break;
			case Embedded::NodeTypes::On_Off:			Evaluate(*(OnOffNode*)data, 0.0f);			break;
			case Embedded::NodeTypes::Invert:			Evaluate(*(InvertNode*)data, 0.0f);			break;
			case Embedded::NodeTypes::Toggle:			Evaluate(*(ToggleNode*)data, 0.0f);			break;
			case Embedded::NodeTypes::Delay:			Evaluate(*(DelayNode*)data, 0.0f);			break;
			case Embedded::NodeTypes::Push_Button:		Evaluate(*(PushButtonNode*)data, 0.0f);		break;
			case Embedded::NodeTypes::Map_Value:		Evaluate(*(MapValueNode*)data, 0.0f);		break;
			case Embedded::NodeTypes::Math:				Evaluate(*(MathNode*)data, 0.0f);			break;
			//case Embedded::NodeTypes::Value:			Evaluate(*(float);
			case Embedded::NodeTypes::Select:			Evaluate(*(SelectNode*)data, 0.0f);			break;
			//case Embedded::NodeTypes::Count:			Evaluate
			//default:									Evaluate
		}
	}

	NodesVector::NodesVector()
	{
	}

	bool NodesVector::add(NodeTypes type)
	{
		if(m_nextIndex > NODE_VECTOR_MAX_SIZE)
			return false;

		m_data[m_nextIndex] = (uint8_t) type;
		NodeVariant var(&m_data[m_nextIndex]);

		if(m_nextIndex + var.getSize() > NODE_VECTOR_MAX_SIZE)
			return false;

		var.initalizeData();
		m_nextIndex += var.getSize();
		m_count++;
		return true;
	}

	NodeVariant NodesVector::at(uint32_t index)
	{
		if(index >= m_count)
			return NodeVariant(nullptr);

		uint8_t* indexIterator = m_data;
		NodeVariant var(indexIterator);

		for(uint32_t indexCount = 0; indexCount != index; indexCount++) {
			var = NodeVariant(indexIterator);
			indexIterator += var.getSize();
		}

		return var;
	}

	NodeVariant NodesVector::next(NodeVariant& var)
	{
		return NodeVariant(var.pointer + var.getSize());
	}

	void NodesVector::evaluateAll()
	{
		NodeVariant v = at(0);
		v.evaluate();
		for(uint32_t i = 1; i < m_count; i++) {
			v = next(v);
			v.evaluate();
		}
	}

}

