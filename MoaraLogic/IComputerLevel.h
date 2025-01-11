#pragma once
#include "EPieceType.h"

#include "IBoard.h"

#include <ranges>
#include <algorithm>
#include <random>

using MovePosition = std::pair<uint8_t, uint8_t>;

class IComputerLevel
{
public:
	virtual uint8_t GetPlacingIndex(const BoardComputerPtr& board, EPieceType computerPieceType) = 0;
	virtual MovePosition GetMovingIndex(const BoardComputerPtr& board, EPieceType computerPieceType) = 0;
	virtual uint8_t GetRemovingIndex(const BoardComputerPtr& board, EPieceType computerPieceType) = 0;

	virtual ~IComputerLevel() = default;
};

using ComputerLevelPtr = std::shared_ptr<IComputerLevel>;