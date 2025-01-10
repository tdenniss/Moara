#pragma once
#include "IBoard.h"
#include "IGameConfig.h"

#include <vector>
class BoardFactory
{
public:
	static IBoardPtr Create(EBoardType boardType,
		PieceTypeList players,
		const BoardConfigMatrix& boardMatrix,
		int piecesToPlace);
	static IBoardPtr Create(EBoardType boardType,
		PieceTypeList players,
		std::ifstream& file);
};

