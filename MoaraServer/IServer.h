#pragma once
#include <string>
#include <SFML/Network.hpp>

using LobbySharedPtr = std::shared_ptr<class Lobby>;

class IServer
{
public:
	virtual bool Init(int port) = 0;

	virtual void Start() = 0;
	virtual void Stop() = 0;

	virtual void SendData(const void *data, int dataSize, int clientId) = 0;

	//TODO: change param to client id
	virtual sf::Socket::Status ReceiveData(void *receivedDataBuffer, sf::TcpSocket &clientSocket) = 0;

	virtual void RemoveLobby(LobbySharedPtr lobby) = 0;

	virtual LobbySharedPtr GetLobbyFromIndex(int lobbyId) = 0;

	virtual ~IServer() = default;
};