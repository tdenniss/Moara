#include "pch.h"
#include "EnumStringMaker.h"
#include "DiagonalsNode.h"

DiagonalsNode::DiagonalsNode()
	: m_top{ nullptr }
	, m_topRight{ nullptr }
	, m_right{ nullptr }
	, m_bottomRight{ nullptr }
	, m_bottom{ nullptr }
	, m_bottomLeft{ nullptr }
	, m_left{ nullptr }
	, m_topLeft{ nullptr }
{
}

INode* DiagonalsNode::Clone() const
{
	return new DiagonalsNode(*this);
}

Nodes DiagonalsNode::GetNeighbours() const
{
	return Nodes({ m_top, m_topRight, m_right, m_bottomRight,
					m_bottom, m_bottomLeft, m_left, m_topLeft });
}

INode* DiagonalsNode::GetNeighbour(ENeighboursPosition position) const
{
	switch (position)
	{
	case ENeighboursPosition::Top:
		return m_top;
	case ENeighboursPosition::Top_Right:
		return m_topRight;
	case ENeighboursPosition::Right:
		return m_right;
	case ENeighboursPosition::Bottom_Right:
		return m_bottomRight;
	case ENeighboursPosition::Bottom:
		return m_bottom;
	case ENeighboursPosition::Bottom_Left:
		return m_bottomLeft;
	case ENeighboursPosition::Left:
		return m_left;
	case ENeighboursPosition::Top_Left:
		return m_topLeft;
	default:
		return nullptr;
	}
}

void DiagonalsNode::SetNeighbour(INode* neighbour, ENeighboursPosition position)
{
	switch (position)
	{
	case ENeighboursPosition::Top:
		m_top = neighbour;
		break;
	case ENeighboursPosition::Top_Right:
		m_topRight = neighbour;
		break;
	case ENeighboursPosition::Right:
		m_right = neighbour;
		break;
	case ENeighboursPosition::Bottom_Right:
		m_bottomRight = neighbour;
		break;
	case ENeighboursPosition::Bottom:
		m_bottom = neighbour;
		break;
	case ENeighboursPosition::Bottom_Left:
		m_bottomLeft = neighbour;
		break;
	case ENeighboursPosition::Left:
		m_left = neighbour;
		break;
	case ENeighboursPosition::Top_Left:
		m_topLeft = neighbour;
		break;
	default:
		break;
	}
}

void DiagonalsNode::SetNeighbours(std::vector<INode*> neighbours)
{
	if (neighbours.size() != 8)
		return;

	m_top = neighbours[0];
	m_topRight = neighbours[1];
	m_right = neighbours[2];
	m_bottomRight = neighbours[3];
	m_bottom = neighbours[4];
	m_bottomLeft = neighbours[5];
	m_left = neighbours[6];
	m_topLeft = neighbours[7];
}

