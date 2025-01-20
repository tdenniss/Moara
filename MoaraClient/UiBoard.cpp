#include "UiBoard.h"
#include "Constants.h"

#include <ranges>
#include <fstream>
#include <algorithm>

struct UiBoardConfig
{
	UiBoardConfig(std::string positionPath, std::string connectionPath, float sizeFactor = 1.0f)
		: positionPath(positionPath)
		, connectionPath(connectionPath)
		, sizeFactor(sizeFactor)
	{
	}

	std::string positionPath;
	std::string connectionPath;
	float sizeFactor;
};

static UiBoardConfig BuildBoardConfig(EBoardType type, EBoardSize size)
{
	if (type == EBoardType::Normal && size == EBoardSize::Normal)
		return UiBoardConfig(NORMAL_BOARD_POSITIONS, NORMAL_BOARD_CONNECTIONS);
	if (type == EBoardType::Normal && size == EBoardSize::Big)
		return UiBoardConfig(NORMAL_BIG_BOARD_POSITIONS, NORMAL_BIG_BOARD_CONNECTIONS, 0.89f);
	if (type == EBoardType::Diagonals && size == EBoardSize::Normal)
		return UiBoardConfig(DIAGONAL_BOARD_POSITIONS, DIAGONAL_BOARD_CONNECTIONS);
	return UiBoardConfig(DIAGONAL_BIG_BOARD_POSITIONS, DIAGONAL_BIG_BOARD_CONNECTIONS, 0.89f);
}

IUiBoard* IUiBoard::Produce(EBoardType type, EBoardSize size, const NodesInfo& nodes)
{
	auto boardConfig = BuildBoardConfig(type, size);

	return new UiBoard(boardConfig.positionPath, boardConfig.connectionPath, nodes, boardConfig.sizeFactor);
}

UiBoard::UiBoard(const std::string& typePath, const std::string& sizePath, const NodesInfo& nodes, float nodesDistanceFactor)
	: m_firstSelectedNodeIndex(-1)
	, m_nodesDistanceFactor(nodesDistanceFactor)
	, m_imagesCache()
{
	m_nodePositions = LoadValues(typePath);
	m_nodeConnections = LoadValues(sizePath);

	InitializeButtons(nodes);
	UpdateNodePositions();
	ConnectButtons();

	this->update();
}

void UiBoard::InitializeButtons(const NodesInfo& nodes)
{
	for (uint8_t index = 0; index < nodes.size(); index++)
	{
		NodeButton* nodeButton = new NodeButton(this);
		nodeButton->SetNodeIndex(index);
		nodeButton->SetType(nodes[index].second);

		m_buttons.push_back(nodeButton);
	}
}

Buttons UiBoard::GetAllButtons() const
{
	return m_buttons;
}

void UiBoard::UpdateNodePositions()
{
	int width = this->width();
	int height = this->height();
	int maxX = 6;
	int maxY = 6;

	int offsetX = (width / (maxX + 1)) * m_nodesDistanceFactor;
	int offsetY = (height / (maxY + 1)) * m_nodesDistanceFactor;

	int baseOffset = 20;
	int minWindowSize = 700;
	int dynamicOffsetX = baseOffset * (width / static_cast<float>(minWindowSize));
	int dynamicOffsetY = baseOffset * (height / static_cast<float>(minWindowSize));

	for (size_t index = 0; index < m_nodePositions.size(); ++index)
	{
		Position pos = m_nodePositions[index];

		int newX = pos.first * offsetX + dynamicOffsetX;
		int newY = pos.second * offsetY + dynamicOffsetY;
		m_buttons[index]->move(newX, newY);
		m_buttons[index]->resize(offsetX, offsetY);
		m_buttons[index]->SetPosition(pos);
	}
}

void UiBoard::SwapPieces(uint8_t fromNodeIndex, uint8_t toNodeIndex, EPieceType fromNodeType)
{
	m_buttons[fromNodeIndex]->SetType(EPieceType::None);
	m_buttons[toNodeIndex]->SetType(fromNodeType);
}

void UiBoard::AddPiece(uint8_t nodeIndex, EPieceType nodeType)
{
	m_buttons[nodeIndex]->SetType(nodeType);
}

void UiBoard::RemovePiece(uint8_t index)
{
	m_buttons[index]->SetType(EPieceType::None);
}

void UiBoard::HighlightNodes(PiecesIndexes indexes)
{
	std::ranges::for_each(indexes, [&](auto index)
		{
			m_buttons[index]->Highlight();
			m_highlightedNodes.push_back(index);
		});
}

int UiBoard::GetFirstSelectedNodeIndex()
{
	return m_firstSelectedNodeIndex;
}

void UiBoard::SetFirstSelectedNodeIndex(int index)
{
	m_firstSelectedNodeIndex = index;
}

std::vector<std::pair<int, int>> UiBoard::LoadValues(const std::string& filePath)
{
	std::ifstream inputFile(filePath);

	Connections pairs;

	std::string line;
	while (std::getline(inputFile, line))
	{
		std::istringstream stream(line);
		int fromNodeIndex;
		int toNodeIndex;

		while (stream >> fromNodeIndex >> toNodeIndex) {

			pairs.emplace_back(fromNodeIndex, toNodeIndex);
		}
	}
	inputFile.close();

	return pairs;
}

void UiBoard::paintEvent(QPaintEvent* event)
{
	QPainter painter(this);

	if (!m_buttons.empty() && !m_nodeConnections.empty())
	{
		std::ranges::for_each(m_nodeConnections, [&](const auto& connection) {
			auto [fromNodeIndex, toNodeIndex] = connection;

			painter.setPen(QPen(Qt::black, 5));

			auto& fromButton = m_buttons[fromNodeIndex];
			auto& toButton = m_buttons[toNodeIndex];

			QPoint from = fromButton->pos() + QPoint(fromButton->width() / 2, fromButton->height() / 2);
			QPoint to = toButton->pos() + QPoint(toButton->width() / 2, toButton->height() / 2);

			painter.drawLine(from, to);
			});
	}
}

void UiBoard::resizeEvent(QResizeEvent* event)
{
	QWidget::resizeEvent(event);
	UpdateNodePositions();
}

void UiBoard::ConnectButtons()
{
	std::ranges::for_each(m_buttons, [&](const auto& button) {
		connect(button, &NodeButton::clicked, this, &UiBoard::OnNodeClicked);
		connect(button, &NodeButton::rightClicked, this, &UiBoard::OnRemoveNodeClicked);
		connect(button, &NodeButton::moveClicked, this, &UiBoard::OnMove);
		connect(button, &NodeButton::highlightMove, this, &UiBoard::OnHighlightMove);
		});
}
void UiBoard::OnNodeClicked(uint8_t index)
{
	emit nodeClicked(index);
}
void UiBoard::OnRemoveNodeClicked(uint8_t index)
{
	ResetNodesHighlight();

	emit removeNodeClicked(index);
}

void UiBoard::ResetNodesHighlight()
{
	std::ranges::for_each(m_highlightedNodes, [&](auto index) {
		m_buttons[index]->ResetHighlight();
		});

	m_highlightedNodes.clear();
}

ImagesCache UiBoard::GetImagesCache() const
{
	return m_imagesCache;
}

NodeButton* UiBoard::GetButton(int index) const
{
	return m_buttons[index];
}

void UiBoard::UpdateBoard(const NodesInfo& nodes)
{
	if (nodes.size() != m_buttons.size())
		return;
	else
	{
		for (size_t index = 0; index < nodes.size(); index++)
		{
			m_buttons[index]->SetType(nodes[index].second);
		}
	}
}

void UiBoard::ResetComputerNodesHighlight()
{
	std::ranges::for_each(m_highlightedComputerNodes, [&](auto index) {
		m_buttons[index]->ResetHighlight();
		});

	m_highlightedComputerNodes.clear();
}

void UiBoard::SetHighlightedNodeIndex(uint8_t index)
{
	m_highlightedComputerNodes.push_back(index);
}

void UiBoard::OnMove(uint8_t fromIndex, uint8_t toIndex)
{
	//ResetNodesHighlight();

	emit move(fromIndex, toIndex);
}

void UiBoard::OnHighlightMove(uint8_t index)
{
	emit highlightMove(index);
}

