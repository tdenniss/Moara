#pragma once
#include <stdint.h>

#include "Constants.h"

struct PiecesInformation
{
	PiecesInformation(uint8_t piecesToPlace = 0)
	{
		this->piecesToPlace = piecesToPlace;
		piecesOnTable = 0;
	}

	uint8_t piecesToPlace;
	uint8_t piecesOnTable;
};