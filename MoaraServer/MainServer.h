#pragma once

#include "IserverHandler.h"
#include "Lobby.h"
#include "JsonMessageHandler.h"

#include <pqxx/pqxx>

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

	void ConnectToDatabase();
	void ReadDatabaseConfig();

private:
	ServerHandlerPtr m_serverHandler;

	pqxx::connection* m_dbConnection;
	std::string m_dbname;
	std::string m_user;
	std::string m_password;
	std::string m_hostaddr;
	int m_port;

	Lobbies m_lobbies;
	int m_lastLobbyId;
	CommandMap m_commands;
};
