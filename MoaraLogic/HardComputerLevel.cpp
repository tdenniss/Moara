#include "pch.h"

#include "HardComputerLevel.h"
#include "EGameState.h"

#include <limits>

uint8_t HardComputerLevel::GetPlacingIndex(const BoardComputerPtr& board, EPieceType computerPieceType)
{
	auto possiblePlaces = board->GetPossiblePlaces();

	for (const auto& possibility : possiblePlaces)
	{
		if (board->CanBeFullLine(possibility, computerPieceType))
			return possibility;
	}

	for (const auto& possibility : possiblePlaces)
	{
		for (int i = 0; i < static_cast<int>(EPieceType::Count); ++i)
		{
			EPieceType pieceType = static_cast<EPieceType>(i);
			if (pieceType == computerPieceType)
				continue;

			if (board->CanBeFullLine(possibility, pieceType))
				return possibility;
		}
	}

	return possiblePlaces[GetRandomNumber(0, static_cast<uint8_t>(possiblePlaces.size()))];
}

MovePosition HardComputerLevel::GetMovingIndex(const BoardComputerPtr& board, EPieceType computerPieceType)
{
	PieceIndexes startIndexes;
	auto nodes = board->GetAllNodes();

	for (const auto& node : nodes)
	{
		if (node->GetPieceType() == computerPieceType &&
			board->GetPossibleMovesFromNode(node->GetIndex(), computerPieceType).size() > 0)
		{
			startIndexes.push_back(node->GetIndex());
		}
	}

	auto resetMove = [&](uint8_t fromIndex, uint8_t toIndex)
		{
			nodes[fromIndex]->SetPiece(nodes[toIndex]->GetPieceType());
			nodes[toIndex]->SetPiece(EPieceType::None);
		};

	for (const auto& fromIndex : startIndexes)
	{
		PieceIndexes possibleMoves = board->GetPossibleMovesFromNode(fromIndex, computerPieceType);
		for (const auto& toIndex : possibleMoves)
		{
			if (board->CanBeFullLine(toIndex, computerPieceType, fromIndex))
			{
				return std::make_pair(fromIndex, toIndex);
			}

			nodes[toIndex]->SetPiece(nodes[fromIndex]->GetPieceType());
			nodes[fromIndex]->SetPiece(EPieceType::None);

			PieceIndexes newPossibleMoves = board->GetPossibleMovesFromNode(toIndex, computerPieceType);
			for (const auto& newToIndex : newPossibleMoves)
			{
				if (board->CanBeFullLine(newToIndex, computerPieceType, toIndex))
				{
					resetMove(fromIndex, toIndex);
					return std::make_pair(fromIndex, toIndex);
				}
			}
			resetMove(fromIndex, toIndex);
		}
	}

	for (const auto& fromIndex : startIndexes)
	{
		PieceIndexes possibleMoves = board->GetPossibleMovesFromNode(fromIndex, computerPieceType);
		for (const auto& toIndex : possibleMoves)
		{
			for (int i = 0; i < static_cast<int>(EPieceType::Count); ++i)
			{
				EPieceType pieceType = static_cast<EPieceType>(i);
				if (pieceType != computerPieceType && board->CanBeFullLine(toIndex, pieceType))
				{
					return std::make_pair(fromIndex, toIndex);
				}
			}

			nodes[toIndex]->SetPiece(nodes[fromIndex]->GetPieceType());
			nodes[fromIndex]->SetPiece(EPieceType::None);

			for (int i = 0; i < static_cast<int>(EPieceType::Count); ++i)
			{
				EPieceType pieceType = static_cast<EPieceType>(i);
				if (pieceType != computerPieceType)
				{
					PieceIndexes newPossibleMoves = board->GetPossibleMovesFromNode(toIndex, computerPieceType);
					for (const auto& newToIndex : newPossibleMoves)
					{
						if (board->CanBeFullLine(newToIndex, pieceType, toIndex))
						{
							resetMove(fromIndex, toIndex);
							return std::make_pair(fromIndex, toIndex);
						}
					}
				}
			}
			resetMove(fromIndex, toIndex);
		}
	}

	uint8_t randomStartIndex = startIndexes[GetRandomNumber(0, static_cast<uint8_t>(startIndexes.size()))];
	const auto& possibleMoves = board->GetPossibleMovesFromNode(randomStartIndex, computerPieceType);
	uint8_t randomEndIndex = possibleMoves[GetRandomNumber(0, static_cast<uint8_t>(possibleMoves.size()))];

	return std::make_pair(randomStartIndex, randomEndIndex);
}

uint8_t HardComputerLevel::GetRemovingIndex(const BoardComputerPtr& board, EPieceType computerPieceType)
{
	auto possibleRemoves = board->GetPossibleRemoves(computerPieceType);

	return possibleRemoves[GetRandomNumber(0, static_cast<uint8_t>(possibleRemoves.size()))];
}

uint8_t HardComputerLevel::GetRandomNumber(uint8_t lowerBound, uint8_t upperBound)
{
	std::random_device rd;
	std::mt19937 mt(rd());
	std::uniform_real_distribution<double> dist(lowerBound, upperBound);

	return static_cast<uint8_t>(dist(mt));
}

