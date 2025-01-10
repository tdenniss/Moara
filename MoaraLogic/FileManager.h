#pragma once
#include "Constants.h"
#include "EBoardType.h"
#include "EBoardSize.h"

class FileManager
{
public:
	static std::string GetDefaultInputFileName(EBoardType boardType = EBoardType::Normal,
		EBoardSize boardSize = EBoardSize::Normal);
};
