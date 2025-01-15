#pragma once
#include "IServer.h"
#include "JsonMessageHandler.h"

using Socket = sf::TcpSocket;
using SocketPtr = Socket *;
using SocketList = std::vector<SocketPtr>;

using Lobbies = std::vector<LobbySharedPtr>;

using Command = std::function<void(void *data, SocketPtr client)>;
using CommandMap = std::unordered_map <std::string, Command>;

class SfmlServer: public IServer
{
public:
	SfmlServer();
	~SfmlServer();

	//IServer
	bool Init(int port) override;

	void Start() override;
	void Stop() override;

	void SendData(const void *data, int dataSize, int clientId) override;
	sf::Socket::Status ReceiveData(void *receivedDataBuffer, sf::TcpSocket &clientSocket) override;

	void RemoveLobby(LobbySharedPtr lobby) override;

	void SetNewDataCallback(std::function<void(void *, SocketPtr)> newDataFunction);

	void OnNewServerData(void *receivedData, SocketPtr client);

	int GetClientIndex(SocketPtr socket);
	int GetLobbyIdFromClient(int clientId);
	LobbySharedPtr GetLobbyFromIndex(int lobbyId) override;

private:
	void ConnectClient();

	void InitCommands();

	bool CheckPlayerTurn(int lobbyId, int clientId, IGamePtr game);

	IGamePtr GetGame(SocketPtr client);

private:
	IGamePtr m_game;
	sf::TcpListener m_listener;
	SocketList m_sockets;
	bool m_running;
	Lobbies m_lobbies;
	int m_lastLobbyId;
	CommandMap m_commands;
	std::function<void(void *, SocketPtr)> m_newDataFunction;
};
