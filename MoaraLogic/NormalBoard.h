#pragma once
#include "Board.h"

class NormalBoard : public Board
{
public:
	NormalBoard();
	NormalBoard(const PieceTypeList& players, const BoardConfigMatrix& boardMatrix, int piecesToPlace);
	NormalBoard(const PieceTypeList& players, std::ifstream& file);

	EBoardType GetBoardType() const override final;

	void LoadFromConfig(const PieceTypeList& players, const BoardConfigMatrix& boardMatrix, int piecesToPlace) override final;
	void LoadBoard(const PieceTypeList& players, std::ifstream& file) override final;
	void SaveBoard(std::ofstream& file) const override final;

	bool IsFullLine(uint8_t nodeIndex, EPieceType currentNodeType = EPieceType::None, uint8_t indexToIgnore = 255, bool windmill = false) const override final;

	bool IsWindmillRule(uint8_t nodeIndex, EPieceType currentNodeType = EPieceType::None) const override final;

	~NormalBoard() override = default;
private:
	void ConfigureNodes(const std::vector<int> indexes);
};
