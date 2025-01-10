#pragma once
#include "INode.h"

class Node : public INode
{
public:
	Node();

	EPieceType GetPieceType() const override final;
	uint8_t GetIndex() const override final;

	void SetPiece(EPieceType type) override final;
	void SetIndex(uint8_t index) override final;

	virtual ~Node() override = default;

protected:
	EPieceType	m_type;
	uint8_t		m_index;
};
