#pragma once

#include "IComputerLevel.h"

class HardComputerLevel : public IComputerLevel
{
public:
	uint8_t GetPlacingIndex(const BoardComputerPtr& board, EPieceType computerPieceType) override;
	MovePosition GetMovingIndex(const BoardComputerPtr& board, EPieceType computerPieceType) override;
	uint8_t GetRemovingIndex(const BoardComputerPtr& board, EPieceType computerPieceType) override;
private:
	uint8_t GetRandomNumber(uint8_t lowerBound, uint8_t upperBound);
};
