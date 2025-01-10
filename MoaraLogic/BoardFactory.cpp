#include "pch.h"
#include "BoardFactory.h"
#include "DiagonalsBoard.h"
#include "NormalBoard.h"

IBoardPtr BoardFactory::Create(EBoardType boardType,
	PieceTypeList players,
	const BoardConfigMatrix& boardMatrix,
	int piecesToPlace)
{
	switch (boardType)
	{
	case EBoardType::Normal:
		return std::make_shared<NormalBoard>(players, boardMatrix, piecesToPlace);
	case EBoardType::Diagonals:
		return std::make_shared<DiagonalsBoard>(players, boardMatrix, piecesToPlace);
	default:
		throw std::invalid_argument("Unknown board type");
	}
}

IBoardPtr BoardFactory::Create(EBoardType boardType,
	PieceTypeList players,
	std::ifstream& file)
{
	switch (boardType)
	{
	case EBoardType::Normal:
		return std::make_shared<NormalBoard>(players, file);
	case EBoardType::Diagonals:
		return std::make_shared<DiagonalsBoard>(players, file);
	default:
		throw std::invalid_argument("Unknown board type");
	}
}
