#include "pch.h"
#include "Board.h"
#include "FileManager.h"

#include <exception>
#include <stdexcept>
#include <ranges>
#include <algorithm>

Board::Board()
	: m_boardState{ EBoardState::None }
	, m_nodes{}
	, m_playersPieces{}
{
}

Board::Board(const PieceTypeList& players, const BoardConfigMatrix& boardMatrix, int piecesToPlace)
	: m_boardState{ EBoardState::None }
	, m_nodes{}
	, m_playersPieces{}
{
}

Board::Board(const PieceTypeList& players, std::ifstream& file)
	: m_boardState{ EBoardState::None }
	, m_nodes{}
	, m_playersPieces{}
{
}

EPieceType Board::GetNodeType(uint8_t nodeIndex) const
{
	return m_nodes[nodeIndex]->GetPieceType();
}

PieceTypeList Board::GetAllNodesType() const
{
	PieceTypeList pieceTypes;
	pieceTypes.reserve(m_nodes.size());

	std::ranges::transform(m_nodes, std::back_inserter(pieceTypes), [](const auto& node) {
		return node->GetPieceType();
		});

	return pieceTypes;
}

NodeList Board::GetAllNodes() const
{
	return m_nodes;
}

uint8_t Board::GetNodesCount() const
{
	return static_cast<uint8_t>(m_nodes.size());
}

void Board::AddPiece(uint8_t nodeIndex, EPlayerType player)
{
	if (m_nodes[nodeIndex]->GetPieceType() != EPieceType::None)
	{
		return;
	}

	m_nodes[nodeIndex]->SetPiece(player);

	if (IsFullLine(nodeIndex))
	{
		m_boardState = EBoardState::Full_line;
	}

	++m_playersPieces[player].piecesOnTable;
	--m_playersPieces[player].piecesToPlace;
}

void Board::RemovePiece(uint8_t nodeIndex, EPlayerType player)
{
	auto curentPieceType = m_nodes[nodeIndex]->GetPieceType();

	if (curentPieceType == EPieceType::None)
		return;
	if (curentPieceType == player)
		return;

	if (!CanRemovePiece(nodeIndex))
		return;

	--m_playersPieces[curentPieceType].piecesOnTable;
	m_nodes[nodeIndex]->SetPiece(EPieceType::None);

	m_boardState = EBoardState::None;
}

void Board::UndoAddPiece(uint8_t nodeIndex)
{
	auto curentPieceType = m_nodes[nodeIndex]->GetPieceType();

	--m_playersPieces[curentPieceType].piecesOnTable;
	++m_playersPieces[curentPieceType].piecesToPlace;
	m_nodes[nodeIndex]->SetPiece(EPieceType::None);

	m_boardState = EBoardState::None;
}

void Board::UndoMovePiece(uint8_t firstNodeIndex, uint8_t secondNodeIndex)
{
	m_nodes[firstNodeIndex]->SetPiece(m_nodes[secondNodeIndex]->GetPieceType());
	m_nodes[secondNodeIndex]->SetPiece(EPieceType::None);

	m_boardState = EBoardState::None;
}

void Board::UndoRemovePiece(uint8_t nodeIndex, EPlayerType player)
{
	m_nodes[nodeIndex]->SetPiece(player);

	++m_playersPieces[player].piecesOnTable;

	m_boardState = EBoardState::None;
}

void Board::RemovePlayerPieces(EPlayerType player)
{
	for (auto& node : m_nodes)
	{
		if (node->GetPieceType() == player)
			node->SetPiece(EPieceType::None);
	}
	m_playersPieces.erase(player);
}

bool Board::CanRemovePiece(uint8_t nodeIndex) const
{
	if (IsFullLine(nodeIndex))
	{
		auto curentPieceType = m_nodes[nodeIndex]->GetPieceType();
		auto indexes = GetSamePieceTypeNodeIndexes(curentPieceType);

		return !std::any_of(indexes.begin(), indexes.end(),
			[this](auto index) {
				return !IsFullLine(index);
			}
		);
	}

	return true;
}

INode* Board::GetNodeFromIndex(uint8_t index) const
{
	if (index < 0 || index >= m_nodes.size())
		throw std::invalid_argument("Index not found");
	return m_nodes[index];
}

uint8_t Board::GetIndexFromNode(INode* node) const
{
	if (auto it = std::ranges::find(m_nodes, node); it != m_nodes.end())
	{
		return static_cast<uint8_t>(std::distance(m_nodes.begin(), it));
	}

	throw std::invalid_argument("Node not found");
}

uint8_t Board::CountSamePiece(ENeighboursPosition direction, INode* node, EPieceType windmillPieceType, uint8_t indexToIgnore, bool windmill) const
{
	if (node->GetNeighbour(direction) == nullptr)
		return 0;
	if (node->GetNeighbour(direction)->GetIndex() == indexToIgnore)
		return 0;
	if (!windmill && node->GetPieceType() != node->GetNeighbour(direction)->GetPieceType())
		return 0;
	if (windmill && node->GetNeighbour(direction)->GetPieceType() != windmillPieceType)
		return 0;
	return 1 + CountSamePiece(direction, node->GetNeighbour(direction), windmillPieceType, indexToIgnore, windmill);
}

void Board::IsMoveValid(uint8_t firstNodeIndex, uint8_t secondNodeIndex, EPlayerType movingPlayer) const
{
	auto firstNode = GetNodeFromIndex(firstNodeIndex);
	auto secondNode = GetNodeFromIndex(secondNodeIndex);

	EPieceType secondNodeType = GetNodeFromIndex(secondNodeIndex)->GetPieceType();

	if (m_nodes[firstNodeIndex]->GetPieceType() == EPieceType::None)
	{
		return;
	}
	if (m_nodes[firstNodeIndex]->GetPieceType() != movingPlayer)
	{
		return;
	}
	if (secondNodeType != EPieceType::None && secondNodeType != movingPlayer)
	{
		return;
	}

	if (secondNodeType == movingPlayer)
	{
		return;
	}

	if (m_playersPieces.at(movingPlayer).piecesOnTable > 3)
	{
		auto firstNodeNeighbours = firstNode->GetNeighbours();

		if (std::ranges::find(firstNodeNeighbours, secondNode) == firstNodeNeighbours.end())
		{
			return;
		}
	}

}

void Board::SetPlayerPiecesOnTable(EPlayerType player, int piecesOnTable)
{
	m_playersPieces[player].piecesOnTable = piecesOnTable;
}

void Board::SetPlayerPiecesToPlace(EPlayerType player, int piecesToPlace)
{
	m_playersPieces[player].piecesToPlace = piecesToPlace;
}

EBoardState Board::GetBoardState() const
{
	return m_boardState;
}

PieceIndexes Board::GetSamePieceTypeNodeIndexes(EPieceType type) const
{
	PieceIndexes typeIndexes;

	for (uint8_t index{ 0 }; index < m_nodes.size(); index++)
	{
		if (m_nodes[index]->GetPieceType() == type)
			typeIndexes.push_back(index);
	}

	return typeIndexes;
}

uint8_t Board::GetPlayerPiecesOnTable(EPlayerType player) const
{
	return m_playersPieces.at(player).piecesOnTable;
}

uint8_t Board::GetPlayerPiecesToPlace(EPlayerType player) const
{
	return m_playersPieces.at(player).piecesToPlace;
}

PieceIndexes Board::GetPossibleMoves(EPlayerType player) const
{
	PieceIndexes possibilities;

	auto isEmpty = [](const auto& n) { return n != nullptr && n->GetPieceType() == EPieceType::None; };

	if (GetPlayerPiecesOnTable(player) > 3)
	{
		for (const auto& boardNode : m_nodes)
			if (boardNode->GetPieceType() == player)
			{
				for (const auto& neighbour : boardNode->GetNeighbours() | std::views::filter(isEmpty))
					possibilities.push_back(neighbour->GetIndex());
			}
	}
	else
	{
		for (const auto& boardNode : m_nodes | std::views::filter(isEmpty))
			possibilities.push_back(boardNode->GetIndex());
	}

	return possibilities;
}

void Board::MovePiece(uint8_t firstNodeIndex, uint8_t secondNodeIndex, EPlayerType player)
{
	IsMoveValid(firstNodeIndex, secondNodeIndex, player);

	m_nodes[secondNodeIndex]->SetPiece(player);
	m_nodes[firstNodeIndex]->SetPiece(EPieceType::None);

	if (IsFullLine(secondNodeIndex) && IsWindmillRule(secondNodeIndex, player))
		m_boardState = EBoardState::Windmill;
	else if (IsFullLine(secondNodeIndex))
		m_boardState = EBoardState::Full_line;
}

Board::~Board()
{
	std::ranges::for_each(m_nodes, [](auto& node) {
		delete node;
		});
}
