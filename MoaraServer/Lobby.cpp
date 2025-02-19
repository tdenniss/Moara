#include "Lobby.h"

#include "JsonMessageHandler.h"
#include "EnumStringMaker.h"

#include <iostream>

Lobby::Lobby(int id, ServerHandlerPtr server)
	: m_game()
	, m_clientIDs()
	, m_lobbyId(id)
	, m_serverHandler(server)
	, m_maxPlayers(2)
{
}

void Lobby::OnMoveMade(uint8_t fromNodeIndex, uint8_t toNodeIndex, EPieceType fromNodeType)
{
	auto [message, size] = JsonMessageHandler::MakeJson("movedPiece"
		, std::make_pair("fromNode", fromNodeIndex)
		, std::make_pair("toNode", toNodeIndex)
		, std::make_pair("nodeType", EPieceTypeStringMaker::GetStringFromEnum(fromNodeType))
	);

	NotifyAllClients(message, size);

}

void Lobby::NotifyAllClients(const void* message, int size)
{
	for (const auto& currentClientId : m_clientIDs)
	{
		if (currentClientId == -1)
			continue;

		m_serverHandler->SendData(message, size, currentClientId);
	}
}

void Lobby::OnAddedPiece(uint8_t addedNodeIndex, EPieceType nodeType)
{
	auto [message, size] = JsonMessageHandler::MakeJson("addedPiece"
		, std::make_pair("index", addedNodeIndex)
		, std::make_pair("nodeType", EPieceTypeStringMaker::GetStringFromEnum(nodeType))
	);

	NotifyAllClients(message, size);
}

void Lobby::OnRemovedPiece(uint8_t removedNodeIndex)
{
	auto [message, size] = JsonMessageHandler::MakeJson("removedPiece"
		, std::make_pair("index", removedNodeIndex)
	);

	NotifyAllClients(message, size);
}

void Lobby::OnGameStateChanged(EGameState newState)
{
	auto [message, size] = JsonMessageHandler::MakeJson("stateChanged"
		, std::make_pair("newState", EGameStateStringMaker::GetStringFromEnum(newState))
	);

	NotifyAllClients(message, size);
}

void Lobby::OnWindmillRule()
{
	auto [message, size] = JsonMessageHandler::MakeJson("windmill");

	NotifyAllClients(message, size);
}

void Lobby::OnPlayerChanged(EPieceType playerType, bool isComputer)
{
	auto [message, size] = JsonMessageHandler::MakeJson("playerChanged"
		, std::make_pair("who", EPieceTypeStringMaker::GetStringFromEnum(playerType))
		, std::make_pair("computer", isComputer)
	);

	NotifyAllClients(message, size);
}

void Lobby::OnPlayerRemoved(EPieceType who)
{
	auto [message, size] = JsonMessageHandler::MakeJson("playerRemoved"
		, std::make_pair("who", EPieceTypeStringMaker::GetStringFromEnum(who))
	);

	NotifyAllClients(message, size);
}

void Lobby::SetGame(IGamePtr game)
{
	m_game = game;

	m_game->AddListener(shared_from_this());
}

IGamePtr Lobby::GetGame()
{
	return m_game;
}

void Lobby::SetNumberOfPlayers(int players)
{
	m_maxPlayers = players;
}

void Lobby::AddClient(int id)
{
	m_clientIDs.push_back(id);
}

void Lobby::RemoveClient(int id)
{
	for (int i = 0; i < m_clientIDs.size(); ++i)
	{
		if (m_clientIDs[i] == id)
		{
			if (m_game)
				m_game->RemovePlayerFromGame(static_cast<EPlayerType>(i));

			m_clientIDs[i] = -1;

			auto [message, size] = JsonMessageHandler::MakeJson("playerLeft");
			NotifyAllClients(message, size);

			return;
		}
	}
}

int Lobby::GetMaxPlayers() const
{
	return m_maxPlayers;
}

int Lobby::GetNumberOfPlayers() const
{
	int numberOfPlayers = 0;
	for (auto player : m_clientIDs)
		if (player != -1)
			++numberOfPlayers;

	return numberOfPlayers;
}

int Lobby::GetClientIndexInLobby(int clientId) const
{
	auto it = std::find(m_clientIDs.begin(), m_clientIDs.end(), clientId);

	if (it != m_clientIDs.end())
	{
		return static_cast<int>(std::distance(m_clientIDs.begin(), it));
	}
	else
	{
		return -1;
	}
}

bool Lobby::IsClientInLobby(int clientId) const
{
	if (std::ranges::find(m_clientIDs, clientId) != m_clientIDs.end())
		return true;
	return false;
}

int Lobby::GetLobbyId() const
{
	return m_lobbyId;
}
