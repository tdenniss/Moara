#pragma once
#include "EPieceType.h"
#include "ENeighboursPosition.h"

#include <vector>
using Nodes = std::vector<class INode*>;

class INode
{
public:
	virtual INode* Clone() const = 0;

	virtual EPieceType GetPieceType() const = 0;
	virtual Nodes GetNeighbours() const = 0;
	virtual INode* GetNeighbour(ENeighboursPosition position) const = 0;
	virtual uint8_t GetIndex() const = 0;

	virtual void SetPiece(EPieceType type) = 0;
	virtual void SetNeighbour(INode* neighbour, ENeighboursPosition position) = 0;
	virtual void SetNeighbours(std::vector<INode*> neighbours) = 0;
	virtual void SetIndex(uint8_t index) = 0;

	virtual ~INode() = 0;
};

inline INode::~INode()
{

}