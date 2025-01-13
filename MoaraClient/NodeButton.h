#pragma once
#include "EPieceType.h"
#include "IUiBoard.h"

#include <QPushButton>
#include <QPainter>

class NodeButton : public QPushButton
{
	Q_OBJECT
public:
	explicit NodeButton(QWidget* parent = nullptr);

	uint8_t GetNodeIndex() const;
	void SetNodeIndex(uint8_t id);

	EPieceType GetType() const;
	void SetType(EPieceType type);

	Position GetPosition() const;

	void Highlight();
	void Highlight(QPen pen);

	void SetPosition(Position pos);
	void ResetHighlight();

	bool operator<(const NodeButton& other) const;
	bool operator==(const NodeButton& other) const;

private:
	EPieceType m_type;

	uint8_t m_nodeIndex;
	Position m_position;

	bool m_isHighlighted;
	QPen m_highlightPen;
};

