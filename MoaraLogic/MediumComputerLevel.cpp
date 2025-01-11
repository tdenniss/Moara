#include "pch.h"

#include "MediumComputerLevel.h"

MediumComputerLevel::MediumComputerLevel(double placeProbability)
	: m_placeProbability(placeProbability)
{
}

uint8_t MediumComputerLevel::GetPlacingIndex(const BoardComputerPtr& board, EPieceType computerPieceType)
{
	auto possiblePlaces = board->GetPossiblePlaces();

	for (const auto& possibility : possiblePlaces)
	{
		if (board->CanBeFullLine(possibility, computerPieceType) && ShouldReturn(m_placeProbability))
			return possibility;
	}

	for (const auto& possibility : possiblePlaces)
	{
		for (int i = 0; i < static_cast<int>(EPieceType::Count); ++i)
		{
			EPieceType pieceType = static_cast<EPieceType>(i);
			if (pieceType == computerPieceType)
				continue;

			if (board->CanBeFullLine(possibility, pieceType) && ShouldReturn(m_placeProbability))
				return possibility;
		}
	}

	return possiblePlaces[GetRandomIndex(0, static_cast<uint8_t>(possiblePlaces.size()))];
}

MovePosition MediumComputerLevel::GetMovingIndex(const BoardComputerPtr& board, EPieceType computerPieceType)
{
	PieceIndexes startIndexes;
	for (const auto& node : board->GetAllNodes())
	{
		if (node->GetPieceType() == computerPieceType)
			if (board->GetPossibleMovesFromNode(node->GetIndex(), computerPieceType).size() > 0)
				startIndexes.push_back(node->GetIndex());
	}

	for (const auto& fromIndex : startIndexes)
	{
		PieceIndexes possibleMoves = board->GetPossibleMovesFromNode(fromIndex, computerPieceType);
		for (const auto& toIndex : possibleMoves)
		{
			if (board->CanBeFullLine(toIndex, computerPieceType, fromIndex))
				return std::make_pair(fromIndex, toIndex);
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
				if (pieceType == computerPieceType)
					continue;

				if (board->CanBeFullLine(toIndex, pieceType))
					return std::make_pair(fromIndex, toIndex);
			}
		}
	}

	uint8_t randomStartIndex = startIndexes[GetRandomIndex(0, static_cast<uint8_t>(startIndexes.size()))];
	const auto& possibleMoves = board->GetPossibleMovesFromNode(randomStartIndex, computerPieceType);
	uint8_t randomEndIndex = possibleMoves[GetRandomIndex(0, static_cast<uint8_t>(possibleMoves.size()))];

	return std::make_pair(randomStartIndex, randomEndIndex);
}

uint8_t MediumComputerLevel::GetRemovingIndex(const BoardComputerPtr& board, EPieceType computerPieceType)
{
	auto possibleRemoves = board->GetPossibleRemoves(computerPieceType);

	return possibleRemoves[GetRandomIndex(0, static_cast<uint8_t>(possibleRemoves.size()))];
}

bool MediumComputerLevel::ShouldReturn(double probability)
{
	static std::random_device rd;
	static std::mt19937 gen(rd());
	std::uniform_real_distribution<> dis(0.0, 1.0);

	return dis(gen) < probability;
}

uint8_t MediumComputerLevel::GetRandomIndex(uint8_t lowerBound, uint8_t upperBound)
{
	std::random_device rd;
	std::mt19937 mt(rd());
	std::uniform_real_distribution<double> dist(lowerBound, upperBound);

	return static_cast<uint8_t>(dist(mt));
}
