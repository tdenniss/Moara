#pragma once
#include "IComputerLevel.h"

class EasyComputerLevel : public IComputerLevel
{
public:
	uint8_t GetPlacingIndex(const BoardComputerPtr& board, EPieceType computerPieceType) override;
	MovePosition GetMovingIndex(const BoardComputerPtr& board, EPieceType computerPieceType) override;
	uint8_t GetRemovingIndex(const BoardComputerPtr& board, EPieceType computerPieceType) override;
private:
	uint8_t GetRandomIndex(uint8_t lowerBound, uint8_t upperBound);
};
