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

void NodeButton::paintEvent(QPaintEvent* event) {


	auto parent = dynamic_cast<IUiBoard*>(this->parent());

	QPainter painter(this);

	painter.setRenderHint(QPainter::Antialiasing);

	painter.setBrush(Qt::white);

	auto penSize = m_isHighlighted ? 5 : 1;
	auto penColor = m_isHighlighted ? m_highlightPen.color() : Qt::black;

	painter.setPen(QPen(penColor, penSize));

	QRect baseRectangle(0, 0, width(), height());
	painter.drawEllipse(baseRectangle);

	int centerX = (width() - PIECE_IMAGE_WIDTH) / 2;
	int centerY = (height() - PIECE_IMAGE_HEIGHT) / 2;

	QRect centerRect(centerX, centerY, PIECE_IMAGE_WIDTH, PIECE_IMAGE_HEIGHT);

	if (m_type != EPieceType::None)
		painter.drawImage(centerRect, parent->GetImagesCache().GetImage(m_type));
}

void NodeButton::mousePressEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton)
	{
		emit clicked(m_nodeIndex);
	}
	else if (event->button() == Qt::RightButton)
	{
		emit rightClicked(m_nodeIndex);
	}
	QPushButton::mousePressEvent(event);
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

void NodeButton::mouseReleaseEvent(QMouseEvent* event) {
	QPushButton::mouseReleaseEvent(event);
}

void NodeButton::mouseDoubleClickEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton)
	{
		auto parent = dynamic_cast<IUiBoard*>(this->parent());

		if (parent->GetFirstSelectedNodeIndex() == -1)
		{
			parent->SetFirstSelectedNodeIndex(m_nodeIndex);
			emit highlightMove(m_nodeIndex);
		}

		else if (parent->GetFirstSelectedNodeIndex() != -1)
		{
			int fromindex = parent->GetFirstSelectedNodeIndex();
			parent->SetFirstSelectedNodeIndex(-1);

			emit moveClicked(static_cast<uint8_t>(fromindex), m_nodeIndex);
		}
	}

}

bool NodeButton::operator<(const NodeButton& other) const {
	return m_nodeIndex < other.m_nodeIndex;
}

bool NodeButton::operator==(const NodeButton& other) const {
	return m_nodeIndex == other.m_nodeIndex && m_position == other.m_position
		&& m_type == other.m_type;
}
