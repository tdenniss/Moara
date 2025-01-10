#pragma once
#include "EBoardType.h"
#include "EBoardSize.h"
#include "EBoardState.h"

#include "IPlayer.h"
#include "INode.h"
#include "IBoardLoader.h"

#include "NormalNode.h"
#include "PiecesInformation.h"

#include <memory>
#include <unordered_map>

using PlayersPieces = std::unordered_map<EPieceType, PiecesInformation>;
using PieceTypeList = std::vector<EPieceType>;
using PieceIndexes = std::vector<uint8_t>;
using IBoardPtr = std::shared_ptr<class IBoard>;
using NodeList = std::vector<class INode*>;

class IBoard : public IBoardLoader
{
public:
	virtual EPieceType GetNodeType(uint8_t nodeIndex) const = 0;
	virtual PieceTypeList GetAllNodesType() const = 0;
	virtual NodeList GetAllNodes() const = 0;
	virtual uint8_t GetNodesCount() const = 0;
	virtual PieceIndexes GetSamePieceTypeNodeIndexes(EPieceType type) const = 0;
	virtual PieceIndexes GetPossibleMoves(EPlayerType player) const = 0;

	virtual EBoardState GetBoardState() const = 0;

	virtual uint8_t GetPlayerPiecesOnTable(EPlayerType player) const = 0;
	virtual uint8_t GetPlayerPiecesToPlace(EPlayerType player) const = 0;

	virtual void AddPiece(uint8_t nodeIndex, EPlayerType player) = 0;
	virtual void MovePiece(uint8_t firstNodeIndex, uint8_t secondNodeIndex, EPlayerType movingPlayer) = 0;
	virtual void RemovePiece(uint8_t nodeIndex, EPlayerType player) = 0;

	virtual void UndoAddPiece(uint8_t nodeIndex) = 0;
	virtual void UndoMovePiece(uint8_t firstNodeIndex, uint8_t secondNodeIndex) = 0;
	virtual void UndoRemovePiece(uint8_t nodeIndex, EPlayerType player) = 0;

	virtual void RemovePlayerPieces(EPlayerType player) = 0;

	virtual void IsMoveValid(uint8_t firstNodeIndex, uint8_t secondNodeIndex, EPlayerType movingPlayer) const = 0;
	virtual bool IsWindmillRule(uint8_t nodeIndex, EPieceType currentNodeType = EPieceType::None) const = 0;
	virtual bool IsFullLine(uint8_t nodeIndex, EPieceType currentNodeType = EPieceType::None, uint8_t indexToIgnore = 255, bool windmill = false) const = 0;

	virtual void SetPlayerPiecesOnTable(EPlayerType player, int piecesOnTable) = 0;
	virtual void SetPlayerPiecesToPlace(EPlayerType player, int piecesToPlace) = 0;

	virtual EBoardType GetBoardType() const = 0;

	virtual ~IBoard() = default;
};
