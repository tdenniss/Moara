#include "pch.h"
#include "NormalNode.h"

NormalNode::NormalNode()
	: m_top{ nullptr }
	, m_right{ nullptr }
	, m_bottom{ nullptr }
	, m_left{ nullptr }
{
}

INode* NormalNode::Clone() const
{
	return new NormalNode(*this);
}

Nodes NormalNode::GetNeighbours() const
{
	return Nodes({ m_top, m_right, m_bottom, m_left });
}

INode* NormalNode::GetNeighbour(ENeighboursPosition position) const
{
	switch (position)
	{
	case ENeighboursPosition::Top:
		return m_top;
	case ENeighboursPosition::Right:
		return m_right;
	case ENeighboursPosition::Bottom:
		return m_bottom;
	case ENeighboursPosition::Left:
		return m_left;
	default:
		return nullptr;
	}
}

void NormalNode::SetNeighbour(INode* neighbour, ENeighboursPosition position)
{
	switch (position)
	{
	case ENeighboursPosition::Top:
		m_top = neighbour;
		break;
	case ENeighboursPosition::Right:
		m_right = neighbour;
		break;
	case ENeighboursPosition::Bottom:
		m_bottom = neighbour;
		break;
	case ENeighboursPosition::Left:
		m_left = neighbour;
		break;
	default:
		break;
	}
}

void NormalNode::SetNeighbours(std::vector<INode*> neighbours)
{
	if (neighbours.size() != 4)
		return;

	m_top = neighbours[0];
	m_right = neighbours[1];
	m_bottom = neighbours[2];
	m_left = neighbours[3];
}
