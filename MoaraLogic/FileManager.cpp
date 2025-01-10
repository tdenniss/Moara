#include "pch.h"
#include "FileManager.h"

std::string FileManager::GetDefaultInputFileName(EBoardType boardType, EBoardSize boardSize)
{
	std::string fileName = "../MoaraLogic/";

	switch (boardType)
	{
	case EBoardType::Normal:
		fileName += "Normal_";
		break;
	case EBoardType::Diagonals:
		fileName += "Diagonals_";
		break;
	default:
		break;
	}
	switch (boardSize)
	{
	case EBoardSize::Normal:
		fileName += "Normal.txt";
		break;
	case EBoardSize::Big:
		fileName += "Big.txt";
		break;
	default:
		break;
	}

	return fileName;
}
