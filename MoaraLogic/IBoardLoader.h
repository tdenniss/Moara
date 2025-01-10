#pragma once
#include "Constants.h"
#include "BoardConfigurations.h"

#include <fstream>
#include <sstream>

using PieceTypeList = std::vector<EPieceType>;

class IBoardLoader
{
public:
	virtual void LoadBoard(const PieceTypeList& players, std::ifstream& file) = 0;
	virtual void LoadFromConfig(const PieceTypeList& players, const BoardConfigMatrix& boardMatrix, int piecesToPlace) = 0;
	virtual void SaveBoard(std::ofstream& file) const = 0;
};