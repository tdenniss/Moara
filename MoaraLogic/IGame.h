#pragma once

#include "EBoardType.h"
#include "EBoardSize.h"
#include "EGameState.h"
#include "EPieceType.h"
#include "EComputerLevel.h"

#include "IGameConfig.h"
#include "IGameListener.h"
#include "IComputerLevel.h"

#include <vector>
#include <string>
#include <memory>

struct PlayerConfig;

using PieceTypeList = std::vector<EPieceType>;
using IGamePtr = std::shared_ptr<class IGame>;
using EPlayerType = EPieceType;
using PlayerTypeList = std::vector<EPlayerType>;
using ListenerSharedPtr = std::shared_ptr<IGameListener>;
using NodeList = std::vector<class INode*>;

class IGame
{
public:
    static IGamePtr Produce(GameConfigPtr& gameConfig);
    virtual void Initialize(GameConfigPtr& gameConfig) = 0;
    virtual EGameState GetGameState() const = 0;
    virtual EPieceType GetWinningPlayer() const = 0;
    virtual EPieceType GetActivePlayer() const = 0;
    virtual int GetActivePlayerIndex() const = 0;
    virtual EPieceType GetPiece(uint8_t nodeIndex) const = 0;
    virtual PieceTypeList GetAllPieces() const = 0;
    virtual EPieceType GetNode(uint8_t nodeIndex) const = 0;
    virtual NodeList GetAllNodes() const = 0;
    virtual PlayerTypeList GetAllPlayerTypes() const = 0;
    virtual uint8_t GetPlayerPiecesOnBoard(EPieceType player) const = 0;
    virtual uint8_t GetPlayerPiecesToPlace(EPieceType player) const = 0;
    virtual PieceIndexes GetPossibleMovesFromNode(uint8_t nodeIndex) const = 0;
    virtual PieceIndexes GetPossibleRemoves() const = 0;
    virtual double GetRoundTime() const = 0;
    virtual void CheckWiningPlayer() = 0;
    virtual void AddListener(ListenerSharedPtr listener) = 0;
    virtual void RemoveListener(ListenerWeakPtr listener) = 0;
    virtual void RemoveAllListeners() = 0;
    virtual void PlacePiece(uint8_t nodeIndex) = 0;
    virtual void MovePiece(uint8_t fromIndex, uint8_t toIndex) = 0;
    virtual void RemovePiece(uint8_t nodeIndex) = 0;
    virtual void Undo() = 0;
    virtual void LoadGame(const std::string& filePath) = 0;
    virtual void SaveGame(const std::string& filePath) const = 0;
    virtual void LetComputerPlay() = 0;
    virtual bool IsComputerTurn() = 0;
    virtual void SetComputerLevel(ComputerLevelPtr computerLevel) = 0;
    virtual void SetComputerLevel(EComputerLevel level) = 0;
    virtual void SetPlayerConfig(PlayerConfig& config) = 0;
    virtual void RemovePlayerFromGame(EPlayerType player) = 0;

    virtual ~IGame() = default;
};
