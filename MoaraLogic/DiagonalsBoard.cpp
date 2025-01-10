#include "pch.h"
#include "DiagonalsBoard.h"
#include "FileManager.h"
#include "DiagonalsNode.h"

#include <exception>
#include <stdexcept>
#include <ranges>
#include <algorithm>

DiagonalsBoard::DiagonalsBoard()
	: Board()
{
	PieceTypeList players;
	players.push_back(EPieceType::White);
	players.push_back(EPieceType::Black);
	players.push_back(EPieceType::Red);

	LoadFromConfig(players, DIAGONALS_NORMAL_BOARD_CONFIG, 9);
}

DiagonalsBoard::DiagonalsBoard(const PieceTypeList& players, const BoardConfigMatrix& boardMatrix, int piecesToPlace)
	: Board(players, boardMatrix, piecesToPlace)
{
	LoadFromConfig(players, boardMatrix, piecesToPlace);
}

DiagonalsBoard::DiagonalsBoard(const PieceTypeList& players, std::ifstream& file)
	: Board()
{
	LoadBoard(players, file);
}

bool DiagonalsBoard::IsFullLine(uint8_t nodeIndex, EPieceType currentNodeType, uint8_t indexToIgnore, bool windmill) const
{
	auto currentNode = m_nodes[nodeIndex];
	uint8_t countCurrentNode = 1;

	if (currentNode->GetPieceType() == EPieceType::None && !windmill)
	{
		return false;
	}

	uint8_t count1 = CountSamePiece(ENeighboursPosition::Top, currentNode, currentNodeType, indexToIgnore, windmill);
	uint8_t count2 = CountSamePiece(ENeighboursPosition::Bottom, currentNode, currentNodeType, indexToIgnore, windmill);

	if (count1 + count2 + countCurrentNode >= FULL_LINE)
		return true;

	count1 = CountSamePiece(ENeighboursPosition::Left, currentNode, currentNodeType, indexToIgnore, windmill);
	count2 = CountSamePiece(ENeighboursPosition::Right, currentNode, currentNodeType, indexToIgnore, windmill);

	if (count1 + count2 + countCurrentNode >= FULL_LINE)
		return true;

	count1 = CountSamePiece(ENeighboursPosition::Top_Right, currentNode, currentNodeType, indexToIgnore, windmill);
	count2 = CountSamePiece(ENeighboursPosition::Bottom_Left, currentNode, currentNodeType, indexToIgnore, windmill);

	if (count1 + count2 + countCurrentNode >= FULL_LINE)
		return true;

	count1 = CountSamePiece(ENeighboursPosition::Bottom_Right, currentNode, currentNodeType, indexToIgnore, windmill);
	count2 = CountSamePiece(ENeighboursPosition::Top_Left, currentNode, currentNodeType, indexToIgnore, windmill);

	if (count1 + count2 + countCurrentNode >= FULL_LINE)
		return true;

	return false;
}

void DiagonalsBoard::LoadFromConfig(const PieceTypeList& players, const BoardConfigMatrix& boardMatrix, int piecesToPlace)
{
	m_boardState = EBoardState::None;

	for (auto& node : m_nodes)
		delete node;
	m_nodes.clear();

	m_playersPieces.clear();
	for (const auto& player : players)
	{
		m_playersPieces[player] = PiecesInformation();
	}

	for (auto& playerPieces : m_playersPieces)
	{
		playerPieces.second = PiecesInformation(piecesToPlace);
	}

	for (uint8_t index = 0; index < boardMatrix.size(); index++)
	{
		INode* node = new DiagonalsNode();
		node->SetIndex(index);
		m_nodes.push_back(node);
	}

	for (uint8_t rowIndex = 0; rowIndex < boardMatrix.size(); rowIndex++)
	{
		ConfigureNodes(boardMatrix[rowIndex]);
	}

}

void DiagonalsBoard::LoadBoard(const PieceTypeList& players, std::ifstream& inputFile)
{

	if (!inputFile.is_open())
	{
		return;
	}

	std::string line;
	std::vector<int> values;

	auto ReadNextInts = [&](std::vector<int>& values) {
		values.clear();
		while (std::getline(inputFile, line)) {
			if (line.empty() || line[0] == '#') {
				continue;
			}
			std::istringstream iss(line);
			int value;
			while (iss >> value) {
				values.push_back(value);
			}
			if (!values.empty()) {
				break;
			}
		}
		};

	m_boardState = EBoardState::None;

	for (auto& node : m_nodes)
		delete node;
	m_nodes.clear();

	m_playersPieces.clear();
	for (const auto& player : players)
	{
		m_playersPieces[player] = PiecesInformation();
	}

	//pieces to place
	ReadNextInts(values);

	if (!values.empty())
	{
		int index = 0;
		for (auto& piece : m_playersPieces)
		{
			piece.second = PiecesInformation(values[index]);
			index++;
		}

	}

	//nodes count
	ReadNextInts(values);
	if (!values.empty()) {
		int nodesCount = values[0];

		for (uint8_t index{ 0 }; index < nodesCount; index++)
		{
			INode* node = new DiagonalsNode();
			node->SetIndex(index);
			m_nodes.push_back(node);
		}
	}

	std::vector<int> nodeValues;
	int size = values[0];
	for (size_t index = 0; index < size; index++)
	{
		ReadNextInts(values);

		nodeValues.clear();
		nodeValues.push_back(values[0]);
		nodeValues.push_back(values[1]);
		nodeValues.push_back(values[2]);
		nodeValues.push_back(values[3]);
		nodeValues.push_back(values[4]);
		nodeValues.push_back(values[5]);
		nodeValues.push_back(values[6]);
		nodeValues.push_back(values[7]);
		nodeValues.push_back(values[8]);
		nodeValues.push_back(values[9]);

		if (nodeValues.size() == 10)
		{
			ConfigureNodes({ nodeValues[0], nodeValues[1], nodeValues[2], nodeValues[3], nodeValues[4], nodeValues[5]
				, nodeValues[6] , nodeValues[7] , nodeValues[8] , nodeValues[9] });
		}
	}
}

void DiagonalsBoard::SaveBoard(std::ofstream& outputFile) const
{
	if (!outputFile.is_open())
	{
		return;
	}

	outputFile << "#number of pieces to place" << std::endl;

	for (const auto& playerPieces : m_playersPieces)
	{
		outputFile << static_cast<int>(playerPieces.second.piecesToPlace) << ' ';
	}

	outputFile << std::endl << "#number of nodes" << std::endl;

	outputFile << m_nodes.size() << '\n';

	outputFile << "#board config" << std::endl;

	for (const auto& node : m_nodes)
	{
		int nodeIndex = node->GetIndex();

		int topNodeIndex = node->GetNeighbour(ENeighboursPosition::Top) ?
			node->GetNeighbour(ENeighboursPosition::Top)->GetIndex() : -1;
		int topRightNodeIndex = node->GetNeighbour(ENeighboursPosition::Top_Right) ?
			node->GetNeighbour(ENeighboursPosition::Top_Right)->GetIndex() : -1;
		int rightNodeIndex = node->GetNeighbour(ENeighboursPosition::Right) ?
			node->GetNeighbour(ENeighboursPosition::Right)->GetIndex() : -1;
		int bottomRightNodeIndex = node->GetNeighbour(ENeighboursPosition::Bottom_Right) ?
			node->GetNeighbour(ENeighboursPosition::Bottom_Right)->GetIndex() : -1;
		int bottomNodeIndex = node->GetNeighbour(ENeighboursPosition::Bottom) ?
			node->GetNeighbour(ENeighboursPosition::Bottom)->GetIndex() : -1;
		int bottomLeftNodeIndex = node->GetNeighbour(ENeighboursPosition::Bottom_Left) ?
			node->GetNeighbour(ENeighboursPosition::Bottom_Left)->GetIndex() : -1;
		int leftNodeIndex = node->GetNeighbour(ENeighboursPosition::Left) ?
			node->GetNeighbour(ENeighboursPosition::Left)->GetIndex() : -1;
		int topLeftNodeIndex = node->GetNeighbour(ENeighboursPosition::Top_Left) ?
			node->GetNeighbour(ENeighboursPosition::Top_Left)->GetIndex() : -1;
		int type = node->GetPieceType() != EPieceType::None ?
			static_cast<int>(node->GetPieceType()) : -1;

		outputFile << nodeIndex << ' ' << topNodeIndex << ' ' << topRightNodeIndex << ' '
			<< rightNodeIndex << ' ' << bottomRightNodeIndex << ' ' << bottomNodeIndex << ' '
			<< bottomLeftNodeIndex << ' ' << leftNodeIndex << ' ' << topLeftNodeIndex << ' '
			<< type << '\n';
	}
}

bool DiagonalsBoard::IsWindmillRule(uint8_t nodeIndex, EPieceType currentNodeType) const
{
	auto topNeighbour = m_nodes[nodeIndex]->GetNeighbour(ENeighboursPosition::Top);
	auto topRightNeighbour = m_nodes[nodeIndex]->GetNeighbour(ENeighboursPosition::Top_Right);
	auto rightNeighbour = m_nodes[nodeIndex]->GetNeighbour(ENeighboursPosition::Right);
	auto bottomRightNeighbour = m_nodes[nodeIndex]->GetNeighbour(ENeighboursPosition::Bottom_Right);
	auto bottomNeighbour = m_nodes[nodeIndex]->GetNeighbour(ENeighboursPosition::Bottom);
	auto bottomLeftNeighbour = m_nodes[nodeIndex]->GetNeighbour(ENeighboursPosition::Bottom_Left);
	auto leftNeighbour = m_nodes[nodeIndex]->GetNeighbour(ENeighboursPosition::Left);
	auto topLeftNeighbour = m_nodes[nodeIndex]->GetNeighbour(ENeighboursPosition::Top_Left);

	if (topNeighbour != nullptr && topNeighbour->GetPieceType() == EPieceType::None && IsFullLine(topNeighbour->GetIndex(), currentNodeType, nodeIndex, true))
		return true;
	if (topRightNeighbour != nullptr && topRightNeighbour->GetPieceType() == EPieceType::None && IsFullLine(topRightNeighbour->GetIndex(), currentNodeType, nodeIndex, true))
		return true;
	if (rightNeighbour != nullptr && rightNeighbour->GetPieceType() == EPieceType::None && IsFullLine(rightNeighbour->GetIndex(), currentNodeType, nodeIndex, true))
		return true;
	if (bottomRightNeighbour != nullptr && bottomRightNeighbour->GetPieceType() == EPieceType::None && IsFullLine(bottomRightNeighbour->GetIndex(), currentNodeType, nodeIndex, true))
		return true;
	if (bottomNeighbour != nullptr && bottomNeighbour->GetPieceType() == EPieceType::None && IsFullLine(bottomNeighbour->GetIndex(), currentNodeType, nodeIndex, true))
		return true;
	if (bottomLeftNeighbour != nullptr && bottomLeftNeighbour->GetPieceType() == EPieceType::None && IsFullLine(bottomLeftNeighbour->GetIndex(), currentNodeType, nodeIndex, true))
		return true;
	if (leftNeighbour != nullptr && leftNeighbour->GetPieceType() == EPieceType::None && IsFullLine(leftNeighbour->GetIndex(), currentNodeType, nodeIndex, true))
		return true;
	if (topLeftNeighbour != nullptr && topLeftNeighbour->GetPieceType() == EPieceType::None && IsFullLine(topLeftNeighbour->GetIndex(), currentNodeType, nodeIndex, true))
		return true;

	return false;
}

void DiagonalsBoard::ConfigureNodes(const std::vector<int> indexes)
{
	int nodeIndex = indexes[0];
	int topNodeIndex = indexes[1];
	int topRightNodeIndex = indexes[2];
	int rightNodeIndex = indexes[3];
	int bottomRightNodeIndex = indexes[4];
	int bottomNodeIndex = indexes[5];
	int bottomLeftNodeIndex = indexes[6];
	int leftNodeIndex = indexes[7];
	int topLeftNodeIndex = indexes[8];
	int type = indexes[9];

	auto node = m_nodes[nodeIndex];

	node->SetIndex(nodeIndex);

	if (topNodeIndex != -1) node->SetNeighbour(m_nodes[topNodeIndex], ENeighboursPosition::Top);
	if (topRightNodeIndex != -1) node->SetNeighbour(m_nodes[topRightNodeIndex], ENeighboursPosition::Top_Right);
	if (rightNodeIndex != -1) node->SetNeighbour(m_nodes[rightNodeIndex], ENeighboursPosition::Right);
	if (bottomRightNodeIndex != -1) node->SetNeighbour(m_nodes[bottomRightNodeIndex], ENeighboursPosition::Bottom_Right);
	if (bottomNodeIndex != -1) node->SetNeighbour(m_nodes[bottomNodeIndex], ENeighboursPosition::Bottom);
	if (bottomLeftNodeIndex != -1) node->SetNeighbour(m_nodes[bottomLeftNodeIndex], ENeighboursPosition::Bottom_Left);
	if (leftNodeIndex != -1) node->SetNeighbour(m_nodes[leftNodeIndex], ENeighboursPosition::Left);
	if (topLeftNodeIndex != -1) node->SetNeighbour(m_nodes[topLeftNodeIndex], ENeighboursPosition::Top_Left);

	if (type == -1)
		return;

	node->SetPiece(static_cast<EPieceType>(type));

	++m_playersPieces[static_cast<EPieceType>(type)].piecesOnTable;
}

EBoardType DiagonalsBoard::GetBoardType() const
{
	return EBoardType::Diagonals;
}