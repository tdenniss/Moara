#pragma once
#include "Node.h"

class NormalNode : public Node
{
public:
	NormalNode();

	INode* Clone() const override final;

	Nodes GetNeighbours() const override final;
	INode* GetNeighbour(ENeighboursPosition position) const override final;

	void SetNeighbour(INode* neighbour, ENeighboursPosition position) override final;
	void SetNeighbours(std::vector<INode*> neighbours) override final;

	~NormalNode() override = default;
private:
	INode* m_top;
	INode* m_left;
	INode* m_bottom;
	INode* m_right;
};
