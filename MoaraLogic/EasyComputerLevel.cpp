#include "pch.h"

#include "EasyComputerLevel.h"

uint8_t EasyComputerLevel::GetPlacingIndex(const BoardComputerPtr& board, EPieceType computerPieceType)
{
	PieceIndexes possiblePlacingIndexes;

	std::ranges::for_each(board->GetAllNodes(), [&possiblePlacingIndexes](auto& node)
		{
			if (node->GetPieceType() == EPieceType::None)
				possiblePlacingIndexes.push_back(node->GetIndex());
		});

	return possiblePlacingIndexes[GetRandomIndex(0, static_cast<uint8_t>(possiblePlacingIndexes.size()))];
}

MovePosition EasyComputerLevel::GetMovingIndex(const BoardComputerPtr& board, EPieceType computerPieceType)
{
	PieceIndexes placedPieces;

	for (const auto& node : board->GetAllNodes())
	{
		if (node->GetPieceType() == computerPieceType)
			placedPieces.push_back(node->GetIndex());
	}

	PieceIndexes possibleMoves;
	uint8_t randomStartIndex;

	if (board->GetPossibleMoves(computerPieceType).size() != 0)
	{
		while (possibleMoves.size() == 0)
		{
			auto randomIndex = GetRandomIndex(0, static_cast<uint8_t>(placedPieces.size()) - 1);

			randomStartIndex = placedPieces[randomIndex];

			possibleMoves = board->GetPossibleMovesFromNode(randomStartIndex, computerPieceType);

			if (possibleMoves.size() == 0)
			{
				placedPieces.erase(placedPieces.begin() + randomIndex);
			}
		}
	}

	return std::make_pair(randomStartIndex, possibleMoves[GetRandomIndex(0, static_cast<uint8_t>(possibleMoves.size()))]);
}

uint8_t EasyComputerLevel::GetRemovingIndex(const BoardComputerPtr& board, EPieceType computerPieceType)
{
	auto possibleRemoves = board->GetPossibleRemoves(computerPieceType);

	return possibleRemoves[GetRandomIndex(0, static_cast<uint8_t>(possibleRemoves.size()))];
}
uint8_t EasyComputerLevel::GetRandomIndex(uint8_t lowerBound, uint8_t upperBound)
{
	std::random_device rd;
	std::mt19937 mt(rd());
	std::uniform_real_distribution<double> dist(lowerBound, upperBound);

	return static_cast<uint8_t>(dist(mt));
}
