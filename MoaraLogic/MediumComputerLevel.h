#pragma once
#include "IComputerLevel.h"

class MediumComputerLevel : public IComputerLevel
{
public:
	MediumComputerLevel(double placeProbability = COMPUTER_RETURN_PROBABILITY);
	uint8_t GetPlacingIndex(const BoardComputerPtr& board, EPieceType computerPieceType) override;
	MovePosition GetMovingIndex(const BoardComputerPtr& board, EPieceType computerPieceType) override;
	uint8_t GetRemovingIndex(const BoardComputerPtr& board, EPieceType computerPieceType) override;
	bool ShouldReturn(double probability);
private:
	uint8_t GetRandomIndex(uint8_t lowerBound, uint8_t upperBound);
	double m_placeProbability;
};
