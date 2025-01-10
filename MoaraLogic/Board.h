#pragma once

#include "IBoard.h"

#include "GameConfig.h"

using BoardPtr = std::shared_ptr<class Board>;

class Board : public IBoard
{
public:
	Board();
	Board(const PieceTypeList& players, const BoardConfigMatrix& boardMatrix, int piecesToPlace);
	Board(const PieceTypeList& players, std::ifstream& file);

	EPieceType GetNodeType(uint8_t nodeIndex) const override final;
	PieceTypeList GetAllNodesType() const override final;
	uint8_t GetNodesCount() const override final;
	PieceIndexes GetSamePieceTypeNodeIndexes(EPieceType type) const override final;

	EBoardState GetBoardState() const override final;

	uint8_t GetPlayerPiecesOnTable(EPlayerType player) const override final;
	uint8_t GetPlayerPiecesToPlace(EPlayerType player) const override final;

	void AddPiece(uint8_t nodeIndex, EPlayerType player) override final;
	void MovePiece(uint8_t firstNodeIndex, uint8_t secondNodeIndex, EPlayerType player) override final;
	void RemovePiece(uint8_t nodeIndex, EPlayerType player) override final;

	NodeList GetAllNodes() const override final;
	PieceIndexes GetPossibleMoves(EPlayerType player) const override final;

	void UndoAddPiece(uint8_t nodeIndex) override final;
	void UndoMovePiece(uint8_t firstNodeIndex, uint8_t secondNodeIndex) override final;
	void UndoRemovePiece(uint8_t nodeIndex, EPlayerType player) override final;
	void RemovePlayerPieces(EPlayerType player) override final;

	void IsMoveValid(uint8_t firstNodeIndex, uint8_t secondNodeIndex, EPlayerType movingPlayer) const override final;

	virtual void SetPlayerPiecesOnTable(EPlayerType player, int piecesOnTable) override final;
	virtual void SetPlayerPiecesToPlace(EPlayerType player, int piecesToPlace) override final;

	virtual ~Board() override;

protected:
	virtual bool CanRemovePiece(uint8_t nodeIndex) const final;
	virtual INode* GetNodeFromIndex(uint8_t index) const final;
	virtual uint8_t GetIndexFromNode(INode* node) const final;
	virtual uint8_t CountSamePiece(ENeighboursPosition direction, INode* node, EPieceType windmillPieceType = EPieceType::None, uint8_t indexToIgnore = 255, bool windmill = false) const final;

protected:
	NodeList m_nodes;
	EBoardState m_boardState;
	PlayersPieces m_playersPieces;
};
