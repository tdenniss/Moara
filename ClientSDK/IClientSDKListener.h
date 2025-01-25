#pragma once
#include "EBoardType.h"
#include "EBoardSize.h"
#include "EComputerLevel.h"
#include "EGameState.h"
#include "EPieceType.h"

#include <vector>
#include <string>

using NodesInfo = std::vector<std::pair<int, EPieceType>>;
using PiecesIndexes = std::vector<uint8_t>;

class IClientSDKListener
{
public:
	virtual void OnSignUpSuccess() = 0;
	virtual void OnLoginSuccess() = 0;

	virtual void OnCreatedLobby(int lobbyId) = 0;
	virtual void OnJoinedLobby(int lobbyId) = 0;

	virtual void OnChangedConfig(EBoardType type, EBoardSize size, EComputerLevel level) = 0;

	virtual void OnGameStarted() = 0;

	virtual void OnError(const std::string& message) = 0;
	virtual void OnInfo(const std::string& message) = 0;
	virtual void OnSetupBoard(NodesInfo nodesInfo) = 0;
	virtual void OnPlayerLeft() = 0;
	virtual void OnUpdateTimer(int timeRemaining) = 0;
	virtual void OnWinningPlayer(const std::string& player) = 0;
	virtual void OnAddedPiece(int index, EPieceType pieceType) = 0;
	virtual void OnMovedPiece(int from, int to, EPieceType pieceType) = 0;
	virtual void OnRemovedPiece(int index) = 0;
	virtual void OnPlayerRemoved(const std::string& player) = 0;
	virtual void OnPlayerChanged(const std::string& player, bool isComputer) = 0;
	virtual void OnStateChanged(EGameState state) = 0;
	virtual void OnWindmill() = 0;
	virtual void OnPossibleMoves(PiecesIndexes indexes) = 0;
	virtual void OnPossibleRemoves(PiecesIndexes indexes) = 0;
	virtual void OnServerDisconnect() = 0;
};