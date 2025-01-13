#include "NodeButton.h"
#include "Constants.h"

#include <QMouseEvent>
#include <QMessageBox>

NodeButton::NodeButton(QWidget* parent)
	: QPushButton(parent)
	, m_nodeIndex(0)
	, m_type(EPieceType::None)
	, m_isHighlighted(false)
	, m_highlightPen(QPen(Qt::red, 5))
{
	setFixedSize(50, 50);
}

void NodeButton::SetNodeIndex(uint8_t id) {
	m_nodeIndex = id;
	update();
}

uint8_t NodeButton::GetNodeIndex() const {
	return m_nodeIndex;
}

void NodeButton::SetType(EPieceType type) {
	m_type = type;
	update();
}

EPieceType NodeButton::GetType() const {
	return m_type;
}

void NodeButton::SetPosition(Position pos) {
	m_position = pos;
	update();
}

Position NodeButton::GetPosition() const {
	return m_position;
}

void NodeButton::Highlight()
{
	m_isHighlighted = true;
	update();
}

void NodeButton::Highlight(QPen pen)
{
	m_isHighlighted = true;
	m_highlightPen = pen;
	update();
}

void NodeButton::ResetHighlight()
{
	if (m_isHighlighted)
	{
		m_isHighlighted = false;
		m_highlightPen = QPen(Qt::red, 5);
		update();
	}
}

bool NodeButton::operator<(const NodeButton& other) const {
	return m_nodeIndex < other.m_nodeIndex;
}

bool NodeButton::operator==(const NodeButton& other) const {
	return m_nodeIndex == other.m_nodeIndex && m_position == other.m_position
		&& m_type == other.m_type;
}
