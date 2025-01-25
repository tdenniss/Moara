#pragma once
#include "IGame.h"

#include "IServerHandler.h"

#include <vector>

using ClientIds = std::vector<int>;
using LobbySharedPtr = std::shared_ptr<class Lobby>;
using Lobbies = std::vector<LobbySharedPtr>;

class Lobby : public IGameListener, public std::enable_shared_from_this<Lobby>
{
public:
	Lobby(int id, ServerHandlerPtr server);

	//IGameListener
	void OnMoveMade(uint8_t fromNodeIndex, uint8_t toNodeIndex, EPieceType fromNodeType) override;
	void OnAddedPiece(uint8_t addedNodeIndex, EPieceType nodeType) override;
	void OnRemovedPiece(uint8_t removedNodeIndex) override;
	void OnGameStateChanged(EGameState newState) override;
	void OnWindmillRule() override;
	void OnPlayerChanged(EPieceType playerType, bool isComputer) override;
	void OnPlayerRemoved(EPieceType who) override;

	void SetGame(IGamePtr game);
	IGamePtr GetGame();

	void SetNumberOfPlayers(int players);

	void NotifyAllClients(const void* json, int size);
	void AddClient(int id);
	void RemoveClient(int id);

	bool IsClientInLobby(int clientId) const;

	int GetMaxPlayers() const;
	int GetNumberOfPlayers() const;
	int GetClientIndexInLobby(int clientId) const;
	int GetLobbyId() const;

private:
	int m_lobbyId;
	ClientIds m_clientIDs;
	IGamePtr m_game;
	ServerHandlerPtr m_serverHandler;
	int m_maxPlayers;
};
