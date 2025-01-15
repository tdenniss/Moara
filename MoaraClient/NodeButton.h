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
protected:
	void paintEvent(QPaintEvent* event) override;
	void mousePressEvent(QMouseEvent* event) override;
	void mouseDoubleClickEvent(QMouseEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;
signals:
	void clicked(uint8_t index);
	void rightClicked(uint8_t index);
	void moveClicked(uint8_t fromIndex, uint8_t toIndex);
	void highlightMove(uint8_t index);
private:
	EPieceType m_type;

	uint8_t m_nodeIndex;
	Position m_position;

	bool m_isHighlighted;
	QPen m_highlightPen;
};

