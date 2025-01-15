#include "SfmlServer.h"
#include "Lobby.h"
#include "InvalidMoveException.h"

#include <iostream>

SfmlServer::SfmlServer()
	: m_running(false)
	, m_lastLobbyId(0)
{
	m_listener.setBlocking(false);

	InitCommands();

	SetNewDataCallback(std::bind(&SfmlServer::OnNewServerData, this, std::placeholders::_1, std::placeholders::_2));
}

SfmlServer::~SfmlServer()
{
	Stop();
	for (auto client : m_sockets)
		delete client;
}

bool SfmlServer::Init(int port)
{
	if (m_listener.listen(port) != sf::Socket::Status::Done)
	{
		return false;
	}
	std::cout << "Server is listening on port " << port << "..." << std::endl;
	return true;
}

void SfmlServer::ConnectClient()
{
	sf::TcpSocket *newClient = new sf::TcpSocket;

	if (m_listener.accept(*newClient) == sf::Socket::Done)
	{
		newClient->setBlocking(false);

		m_sockets.push_back(newClient);
		std::cout << "Client connected: " << newClient->getRemoteAddress() << std::endl;
	} else
		delete newClient;
}

void SfmlServer::InitCommands()
{
	m_commands = {
	{"createLobby", [this](void *data, SocketPtr client)
	{
		auto clientId = GetClientIndex(client);
		auto lobbyId = GetLobbyIdFromClient(clientId);

		if (lobbyId != -1)
			m_lobbies[lobbyId]->RemoveClient(clientId);

		auto lobby = std::make_shared<Lobby>(m_lastLobbyId, this);
		m_lobbies.push_back(lobby);
		GetLobbyFromIndex(m_lastLobbyId)->AddClient(GetClientIndex(client));

		auto [message, size] = JsonMessageHandler::MakeJson("createdLobby", std::make_pair("createdLobbyId", m_lastLobbyId));
		SendData(message, size, GetClientIndex(client));

		++m_lastLobbyId;
	}},

	{"startGame", [this](void *data, SocketPtr client)
	{
		auto clientId = GetClientIndex(client);
		auto lobbyId = GetLobbyIdFromClient(clientId);

		int numberOfPlayers = JsonMessageHandler::GetFromParam<int>("numberOfPlayers", data);

		if (lobbyId == -1)
		{
			auto [message, size] = JsonMessageHandler::MakeJson("error",
				std::make_pair("errorMessage", "Create a lobby first"));

			SendData(message, size, GetClientIndex(client));
			return;
		}

		if (m_lobbies[lobbyId]->GetMaxPlayers() != numberOfPlayers)
			m_lobbies[lobbyId]->SetNumberOfPlayers(numberOfPlayers);

		auto config = JsonMessageHandler::BuildGameConfig(data);
		auto game = IGame::Produce(config);

		m_lobbies[lobbyId]->SetGame(game);

		auto [message, size] = JsonMessageHandler::MakeJson("gameStarted"
			, std::make_pair("parameters", ""));

		m_lobbies[lobbyId]->NotifyAllClients(message, size);
	}},

	{"joinLobby", [this](void *data, SocketPtr client)
	{
		auto lobbyId = JsonMessageHandler::GetFromParam<int>("lobbyId", data);

		if (GetLobbyIdFromClient(GetClientIndex(client)) != -1)
			m_lobbies[GetLobbyIdFromClient(GetClientIndex(client))]->RemoveClient(GetClientIndex(client));

		bool foundLobby = false;
		for (auto &lobby : m_lobbies)
		{
			if (lobby->GetLobbyId() == lobbyId)
			{
				foundLobby = true;
				break;
			}
		}

		if (foundLobby)
		{
			m_lobbies[lobbyId]->AddClient(GetClientIndex(client));

			auto [message, size] = JsonMessageHandler::MakeJson("joinedLobby"
				, std::make_pair("lobbyId", lobbyId));

			SendData(message, size, GetClientIndex(client));
		} else
		{
			auto [message, size] = JsonMessageHandler::MakeJson("error"
				, std::make_pair("errorMessage", "Lobby inexistent"));

			SendData(message, size, GetClientIndex(client));
		}
	}},

	{"getActivePlayer", [this](void *data, SocketPtr client)
	{
		int clientId = GetClientIndex(client);
		int lobbyId = GetLobbyIdFromClient(clientId);
		auto game = m_lobbies[lobbyId]->GetGame();

		auto player = game->GetActivePlayer();

		auto [message, size] = JsonMessageHandler::MakeJson("playerChanged"
			, std::make_pair("player", EPieceTypeStringMaker::GetStringFromEnum(player))
			, std::make_pair("computer", game->IsComputerTurn()));

		SendData(message, size, clientId);
	}},

	{"setupBoard", [this](void *data, SocketPtr client)
	{
		int clientId = GetClientIndex(client);
		int lobbyId = GetLobbyIdFromClient(clientId);
		auto game = m_lobbies[lobbyId]->GetGame();

		auto [message, size] = JsonMessageHandler::BuildNodesVector(game->GetAllNodes());

		SendData(message, size, clientId);
	}},

	{"getPossibleMoves", [this](void *data, SocketPtr client)
	{
		int clientId = GetClientIndex(client);
		int lobbyId = GetLobbyIdFromClient(clientId);
		auto game = m_lobbies[lobbyId]->GetGame();

		auto indexes = game->GetPossibleMovesFromNode(JsonMessageHandler::GetFromParam<int>("fromIndex", data));

		auto [message, size] = JsonMessageHandler::MakeJson("possibleMoves"
			, std::make_pair("indexes", indexes));

		SendData(message, size, clientId);
	}},

	{"getPossibleRemoves", [this](void *data, SocketPtr client)
	{
		int clientId = GetClientIndex(client);
		int lobbyId = GetLobbyIdFromClient(clientId);
		auto game = m_lobbies[lobbyId]->GetGame();

		auto indexes = game->GetPossibleRemoves();

		auto [message, size] = JsonMessageHandler::MakeJson("possibleRemoves"
			, std::make_pair("indexes", indexes));

		SendData(message, size, clientId);
	}},

	{"getTimer", [this](void *data, SocketPtr client)
	{
		int clientId = GetClientIndex(client);
		int lobbyId = GetLobbyIdFromClient(clientId);
		if (lobbyId == -1)
			return;

		auto game = m_lobbies[lobbyId]->GetGame();

		double time = game->GetRoundTime();

		auto [message, size] = JsonMessageHandler::MakeJson("getTimer"
			, std::make_pair("time", static_cast<int>(time)));

		SendData(message, size, clientId);
	}},

	{"getWinningPlayer", [this](void *data, SocketPtr client)
	{
		int clientId = GetClientIndex(client);
		int lobbyId = GetLobbyIdFromClient(clientId);
		auto game = m_lobbies[lobbyId]->GetGame();

		auto winningPlayer = game->GetWinningPlayer();

		auto [message, size] = JsonMessageHandler::MakeJson("winningPlayer"
			, std::make_pair("player", EPieceTypeStringMaker::GetStringFromEnum(winningPlayer)));

		SendData(message, size, clientId);
	}},

	{"movePiece", [this](void *data, SocketPtr client)
	{
		int clientId = GetClientIndex(client);
		int lobbyId = GetLobbyIdFromClient(clientId);
		auto game = m_lobbies[lobbyId]->GetGame();

		if (!CheckPlayerTurn(lobbyId, clientId, game))
			return;

		auto fromIndex = JsonMessageHandler::GetFromParam<int>("fromIndex", data);
		auto toIndex = JsonMessageHandler::GetFromParam<int>("toIndex", data);

		game->MovePiece(fromIndex, toIndex);
	}},

	{"placePiece", [this](void *data, SocketPtr client)
	{
		int clientId = GetClientIndex(client);
		int lobbyId = GetLobbyIdFromClient(clientId);
		auto game = m_lobbies[lobbyId]->GetGame();

		if (!CheckPlayerTurn(lobbyId, clientId, game))
			return;

		auto index = JsonMessageHandler::GetFromParam<int>("index", data);

		game->PlacePiece(index);
	}},

	{"removePiece", [this](void *data, SocketPtr client)
	{
		int clientId = GetClientIndex(client);
		int lobbyId = GetLobbyIdFromClient(clientId);
		auto game = m_lobbies[lobbyId]->GetGame();

		if (!CheckPlayerTurn(lobbyId, clientId, game))
			return;

		auto index = JsonMessageHandler::GetFromParam<int>("index", data);

		game->RemovePiece(index);
	}},

	{"undoAction", [this](void *data, SocketPtr client)
	{
		auto game = GetGame(client);

		game->Undo();
	}},

	{ "changedConfig", [this](void *data, SocketPtr client)
	{
		int clientId = GetClientIndex(client);
		int lobbyId = GetLobbyIdFromClient(clientId);

		if (lobbyId == -1)
		{
			auto [message, size] = JsonMessageHandler::MakeJson("error"
				, std::make_pair("errorMessage", "Not in a lobby")
			);

			SendData(message, size, clientId);
			return;
		}

		m_lobbies[lobbyId]->NotifyAllClients(data, static_cast<int>(std::strlen((char *)data)));
	}}
	};
}

bool SfmlServer::CheckPlayerTurn(int lobbyId, int clientId, IGamePtr game)
{
	if (m_lobbies[lobbyId]->GetClientIndexInLobby(clientId) != game->GetActivePlayerIndex())
	{
		auto [message, size] = JsonMessageHandler::MakeJson("error"
															, std::make_pair("errorMessage", "Not your turn")
		);

		SendData(message, size, clientId);
		return false;
	}

	return true;
}

IGamePtr SfmlServer::GetGame(SocketPtr client)
{
	int clientId = GetClientIndex(client);
	int lobbyId = GetLobbyIdFromClient(clientId);
	return m_lobbies[lobbyId]->GetGame();
}

void SfmlServer::Start()
{
	m_running = true;

	while (m_running)
	{
		ConnectClient();

		for (auto it = m_sockets.begin(); it != m_sockets.end(); )
		{
			SocketPtr client = *it;
			if (client->getRemoteAddress() != sf::IpAddress::None)
			{
				sf::SocketSelector selector;
				selector.add(*client);

				if (selector.wait(sf::milliseconds(50)))
				{
					if (selector.isReady(*client))
					{
						void *receivedDataBuffer = new char[1024];
						auto status = ReceiveData(receivedDataBuffer, *client);

						int lobbyIndex;

						switch (status)
						{
							case sf::Socket::Done:
								std::cout << "Received " << (char *)receivedDataBuffer << " bytes: " << std::strlen((char *)receivedDataBuffer) << std::endl;

								try
								{
									m_newDataFunction(receivedDataBuffer, client);
								} catch (const std::exception &e)
								{
									std::cerr << "Error parsing JSON: " << e.what() << std::endl;
								}
								break;
							case sf::Socket::Disconnected:
								std::cout << "Client disconnected" << std::endl;

								lobbyIndex = GetLobbyIdFromClient(GetClientIndex(client));
								if (lobbyIndex != -1)
									m_lobbies[lobbyIndex]->RemoveClient(GetClientIndex(client));

								delete client;
								it = m_sockets.erase(it);

								delete[](char *)receivedDataBuffer;
								continue;
							case sf::Socket::NotReady:
							case sf::Socket::Partial:
							case sf::Socket::Error:
							default:
								std::cerr << "Error: Could not receive data" << std::endl;
						}

						delete[](char *)receivedDataBuffer;
					}
				}
			}
			++it;
		}
	}
}

void SfmlServer::SendData(const void *data, int dataSize, int clientId)
{
	std::size_t totalSent = 0;
	std::size_t dataSize_t = dataSize + 1;

	std::cout << "Sent: " << (char *)data << " to " << clientId << std::endl;

	while (totalSent < dataSize_t)
	{
		std::size_t sent;
		sf::Socket::Status status = m_sockets[clientId]->send((char *)data + totalSent, dataSize_t - totalSent, sent);
		if (status == sf::Socket::Status::Disconnected)
		{
			std::cerr << "Client disconnected" << std::endl;
			m_sockets.erase(m_sockets.begin() + clientId);
			return;
		} else if (status == sf::Socket::Status::Error)
		{
			std::cerr << "Error: Could not send data" << std::endl;
			return;
		}
		totalSent += sent;
	}
}

sf::Socket::Status SfmlServer::ReceiveData(void *receivedDataBuffer, sf::TcpSocket &clientSocket)
{
	std::size_t totalReceived = 0;
	sf::Socket::Status status;

	do
	{
		status = clientSocket.receive(receivedDataBuffer, std::strlen((char *)receivedDataBuffer), totalReceived);

		if (status != sf::Socket::Done && status != sf::Socket::Partial)
			break;

	} while (status == sf::Socket::Partial || totalReceived == std::strlen((char *)receivedDataBuffer));

	return status;
}

void SfmlServer::RemoveLobby(LobbySharedPtr lobby)
{
	for (auto it = m_lobbies.begin(); it != m_lobbies.end();)
	{
		if (*it == lobby)
		{
			it = m_lobbies.erase(it);
		} else
		{
			++it;
		}
	}
}

void SfmlServer::SetNewDataCallback(std::function<void(void *, SocketPtr)> newDataFunction)
{
	m_newDataFunction = newDataFunction;
}

void SfmlServer::OnNewServerData(void *receivedData, SocketPtr client)
{
	std::string notificationType = JsonMessageHandler::GetFromCommand<std::string>("action", receivedData);

	auto callbackIt = m_commands.find(notificationType);

	if (callbackIt != m_commands.end())
	{
		try
		{
			m_commands[notificationType](receivedData, client);
		} catch (InvalidMoveException e)
		{
			auto [message, size] = JsonMessageHandler::MakeJson("error"
																, std::make_pair("errorMessage", e.what())
			);

			SendData(message, size, GetClientIndex(client));
		}
	}
}

int SfmlServer::GetClientIndex(SocketPtr socket)
{
	for (auto it = m_sockets.begin(); it != m_sockets.end(); )
	{
		if (socket == *it)
			return static_cast<int>(std::distance(m_sockets.begin(), it));
		++it;
	}
	return -1;
}

int SfmlServer::GetLobbyIdFromClient(int clientId)
{
	for (auto it = m_lobbies.begin(); it != m_lobbies.end(); ++it)
	{
		if (it->get()->IsClientInLobby(clientId))
			return static_cast<int>(std::distance(m_lobbies.begin(), it));
	}

	return -1;
}

LobbySharedPtr SfmlServer::GetLobbyFromIndex(int lobbyId)
{
	for (const auto &lobby : m_lobbies)
	{
		if (lobby->GetLobbyId() == lobbyId)
			return lobby;
	}
	return {};
}

void SfmlServer::Stop()
{
	m_running = false;
}
