#include "pch.h"
#include "Node.h"

Node::Node()
	: m_type{ EPieceType::None }
	, m_index{ 0 }
{
}

EPieceType Node::GetPieceType() const
{
	return m_type;
}

void Node::SetPiece(EPieceType type)
{
	m_type = type;
}

void Node::SetIndex(uint8_t index)
{
	m_index = index;
}

uint8_t Node::GetIndex() const
{
	return m_index;
}
