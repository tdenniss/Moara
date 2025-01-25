#pragma once

#include "IserverHandler.h"
#include "Lobby.h"
#include "JsonMessageHandler.h"

using Command = std::function<void(void* data, int clientId)>;
using CommandMap = std::unordered_map <std::string, Command>;

class MainServer
{
public:
	bool Start(unsigned short port);

private:
	void OnMessageReceived(const void* data, int size, int clientId);
	void OnClientDisconnected(int clientId);

	void InitializeCommands();

	bool CheckPlayerTurn(int lobbyId, int clientId, IGamePtr game);
	LobbySharedPtr GetLobbyFromIndex(int lobbyId);
	int GetLobbyIdFromClient(int clientId);
	void RemoveLobby(LobbySharedPtr lobby);
	IGamePtr GetGame(int clientId);

private:
	ServerHandlerPtr m_serverHandler;

	Lobbies m_lobbies;
	int m_lastLobbyId;
	CommandMap m_commands;
};
