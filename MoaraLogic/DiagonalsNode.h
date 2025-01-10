#pragma once
#include "Node.h"

class DiagonalsNode : public Node
{
public:
	DiagonalsNode();

	INode* Clone() const override final;

	Nodes GetNeighbours() const override final;
	INode* GetNeighbour(ENeighboursPosition position) const override final;

	void SetNeighbour(INode* neighbour, ENeighboursPosition position) override final;
	void SetNeighbours(std::vector<INode*> neighbours) override final;

	~DiagonalsNode() override = default;
private:
	INode* m_top;
	INode* m_topRight;
	INode* m_left;
	INode* m_bottomRight;
	INode* m_bottom;
	INode* m_bottomLeft;
	INode* m_right;
	INode* m_topLeft;
};
