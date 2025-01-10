#include "pch.h"
#include "NormalBoard.h"
#include "FileManager.h"

#include <exception>
#include <stdexcept>
#include <ranges>
#include <algorithm>


NormalBoard::NormalBoard()
	: Board()
{
	PieceTypeList players;
	players.push_back(EPieceType::White);
	players.push_back(EPieceType::Black);

	LoadFromConfig(players, NORMAL_BOARD_CONFIG, 9);
}

NormalBoard::NormalBoard(const PieceTypeList& players, const BoardConfigMatrix& boardMatrix, int piecesToPlace)
	: Board(players, boardMatrix, piecesToPlace)
{
	LoadFromConfig(players, boardMatrix, piecesToPlace);
}

NormalBoard::NormalBoard(const PieceTypeList& players, std::ifstream& file)
	: Board(players, file)
{
	LoadBoard(players, file);
}

bool NormalBoard::IsFullLine(uint8_t nodeIndex, EPieceType currentNodeType, uint8_t indexToIgnore, bool windmill) const
{
	auto currentNode = m_nodes[nodeIndex];
	uint8_t countCurrentNode = 1;

	if (currentNode->GetPieceType() == EPieceType::None && !windmill)
	{
		return false;
	}

	uint8_t countUp = CountSamePiece(ENeighboursPosition::Top, currentNode, currentNodeType, indexToIgnore, windmill);
	uint8_t countDown = CountSamePiece(ENeighboursPosition::Bottom, currentNode, currentNodeType, indexToIgnore, windmill);

	if (countDown + countUp + countCurrentNode >= FULL_LINE)
		return true;

	uint8_t countLeft = CountSamePiece(ENeighboursPosition::Left, currentNode, currentNodeType, indexToIgnore, windmill);
	uint8_t countRight = CountSamePiece(ENeighboursPosition::Right, currentNode, currentNodeType, indexToIgnore, windmill);

	if (countLeft + countRight + countCurrentNode >= FULL_LINE)
		return true;

	return false;
}

void NormalBoard::LoadFromConfig(const PieceTypeList& players, const BoardConfigMatrix& boardMatrix, int piecesToPlace)
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
		INode* node = new NormalNode();
		node->SetIndex(index);
		m_nodes.push_back(node);
	}

	for (uint8_t rowIndex = 0; rowIndex < boardMatrix.size(); rowIndex++)
	{
		ConfigureNodes(boardMatrix[rowIndex]);
	}
}

void NormalBoard::LoadBoard(const PieceTypeList& players, std::ifstream& inputFile)
{
	if (!inputFile.is_open())
	{
		return;
	}

	m_boardState = EBoardState::None;

	for (auto& node : m_nodes)
		delete node;
	m_nodes.clear();

	m_playersPieces.clear();
	for (const auto& player : players)
	{
		m_playersPieces[player] = PiecesInformation();
	}

	std::string line;
	std::vector<int> values;

	auto ReadNextInts = [&](std::vector<int>& values)
		{
			values.clear();
			while (std::getline(inputFile, line))
			{
				if (line.empty() || line[0] == '#')
				{
					continue;
				}
				std::istringstream iss(line);
				int value;
				while (iss >> value)
				{
					values.push_back(value);
				}
				if (!values.empty())
				{
					break;
				}
			}
		};

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
	if (!values.empty())
	{
		int nodesCount = values[0];
		for (uint8_t index{ 0 }; index < nodesCount; index++)
		{
			INode* node = new NormalNode();
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

		if (nodeValues.size() == 6)
		{
			ConfigureNodes({ nodeValues[0], nodeValues[1], nodeValues[2], nodeValues[3], nodeValues[4], nodeValues[5] });
		}
	}

}

void NormalBoard::SaveBoard(std::ofstream& outputFile) const
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

		int upNodeIndex = node->GetNeighbour(ENeighboursPosition::Top) ?
			node->GetNeighbour(ENeighboursPosition::Top)->GetIndex() : -1;
		int rightNodeIndex = node->GetNeighbour(ENeighboursPosition::Right) ?
			node->GetNeighbour(ENeighboursPosition::Right)->GetIndex() : -1;
		int downNodeIndex = node->GetNeighbour(ENeighboursPosition::Bottom) ?
			node->GetNeighbour(ENeighboursPosition::Bottom)->GetIndex() : -1;
		int leftNodeIndex = node->GetNeighbour(ENeighboursPosition::Left) ?
			node->GetNeighbour(ENeighboursPosition::Left)->GetIndex() : -1;
		int type = node->GetPieceType() != EPieceType::None ?
			static_cast<int>(node->GetPieceType()) : -1;

		outputFile << nodeIndex << ' ' << upNodeIndex << ' ' << rightNodeIndex << ' '
			<< downNodeIndex << ' ' << leftNodeIndex << ' ' << type << '\n';
	}
}

bool NormalBoard::IsWindmillRule(uint8_t nodeIndex, EPieceType currentNodeType) const
{
	auto topNeighbour = m_nodes[nodeIndex]->GetNeighbour(ENeighboursPosition::Top);
	auto downNeighbour = m_nodes[nodeIndex]->GetNeighbour(ENeighboursPosition::Bottom);
	auto leftNeighbour = m_nodes[nodeIndex]->GetNeighbour(ENeighboursPosition::Left);
	auto rightNeighbour = m_nodes[nodeIndex]->GetNeighbour(ENeighboursPosition::Right);

	if (topNeighbour != nullptr && topNeighbour->GetPieceType() == EPieceType::None && IsFullLine(topNeighbour->GetIndex(), currentNodeType, nodeIndex, true))
		return true;
	if (downNeighbour != nullptr && downNeighbour->GetPieceType() == EPieceType::None && IsFullLine(downNeighbour->GetIndex(), currentNodeType, nodeIndex, true))
		return true;
	if (leftNeighbour != nullptr && leftNeighbour->GetPieceType() == EPieceType::None && IsFullLine(leftNeighbour->GetIndex(), currentNodeType, nodeIndex, true))
		return true;
	if (rightNeighbour != nullptr && rightNeighbour->GetPieceType() == EPieceType::None && IsFullLine(rightNeighbour->GetIndex(), currentNodeType, nodeIndex, true))
		return true;

	return false;
}

void NormalBoard::ConfigureNodes(const std::vector<int> indexes)
{
	int nodeIndex = indexes[0];
	int topNodeIndex = indexes[1];
	int rightNodeIndex = indexes[2];
	int bottomNodeIndex = indexes[3];
	int leftNodeIndex = indexes[4];
	int type = indexes[5];

	auto node = m_nodes[nodeIndex];

	node->SetIndex(nodeIndex);

	if (topNodeIndex != -1) node->SetNeighbour(m_nodes[topNodeIndex], ENeighboursPosition::Top);
	if (rightNodeIndex != -1) node->SetNeighbour(m_nodes[rightNodeIndex], ENeighboursPosition::Right);
	if (bottomNodeIndex != -1) node->SetNeighbour(m_nodes[bottomNodeIndex], ENeighboursPosition::Bottom);
	if (leftNodeIndex != -1) node->SetNeighbour(m_nodes[leftNodeIndex], ENeighboursPosition::Left);

	if (type == -1)
		return;

	node->SetPiece(static_cast<EPieceType>(type));

	++m_playersPieces[static_cast<EPieceType>(type)].piecesOnTable;
}

EBoardType NormalBoard::GetBoardType() const
{
	return EBoardType::Normal;
}
