#include "MainServer.h"

#include <sstream>

#include "QtServerHandler.h"
#include <iostream>

bool MainServer::Start(unsigned short port)
{
	m_serverHandler = new QtServerHandler();

	InitializeCommands();

	bool ok = m_serverHandler->StartServer(
		port,
		[this](const void* data, int size, int clientId)
		{
			this->OnMessageReceived(data, size, clientId);
		},
		[this](int clientId)
		{
			this->OnClientDisconnected(clientId);
		});

	if (!ok)
	{
		delete m_serverHandler;
		m_serverHandler = nullptr;
		return false;
	}

	ConnectToDatabase();

	return true;
}

void MainServer::OnMessageReceived(const void* data, int size, int clientId)
{
	std::string notificationType = JsonMessageHandler::GetFromCommand<std::string>("action", data);

	auto callbackIt = m_commands.find(notificationType);

	if (callbackIt != m_commands.end())
	{
		m_commands[notificationType](const_cast<void *>(data), clientId);
	}
}

void MainServer::OnClientDisconnected(int clientId)
{
	int lobbyIndex = GetLobbyIdFromClient(clientId);
	if (lobbyIndex != -1)
	{
		m_lobbies[lobbyIndex]->RemoveClient(clientId);

		if(m_lobbies[lobbyIndex]->GetNumberOfPlayers() == 0)
			RemoveLobby(GetLobbyFromIndex(lobbyIndex));
	}
}

void MainServer::InitializeCommands()
{
	m_commands = {
	{"signUp", [this](void* data, int clientId)
	{
		auto username = JsonMessageHandler::GetFromParam<std::string>("username", data);
		auto password = JsonMessageHandler::GetFromParam<std::string>("password", data);

		pqxx::work W(*m_dbConnection);

		std::string sql = "SELECT add_user(" + W.quote(username) + ", " + W.quote(password) + ")";
		pqxx::result R = W.exec(sql);

		if (R[0][0].as<bool>())
		{
			auto [message, size] = JsonMessageHandler::MakeJson("signUpSuccess");
			m_serverHandler->SendData(message, size, clientId);
		}
		else
		{
			auto [message, size] = JsonMessageHandler::MakeJson("error"
				, std::make_pair("errorMessage", "User already exists"));
			m_serverHandler->SendData(message, size, clientId);
		}

		W.commit();
	}},

	{"login", [this](void* data, int clientId)
	{
		auto username = JsonMessageHandler::GetFromParam<std::string>("username", data);
		auto password = JsonMessageHandler::GetFromParam<std::string>("password", data);

		pqxx::work W(*m_dbConnection);

		std::string sql = "SELECT check_credentials(" + W.quote(username) + ", " + W.quote(password) + ")";
		pqxx::result R = W.exec(sql);

		if (R[0][0].as<bool>())
		{
			auto [message, size] = JsonMessageHandler::MakeJson("loginSuccess");
			m_serverHandler->SendData(message, size, clientId);
		}
		else
		{
			auto [message, size] = JsonMessageHandler::MakeJson("error"
				, std::make_pair("errorMessage", "Wrong credentials"));
			m_serverHandler->SendData(message, size, clientId);
		}

		W.commit();

		//verificam daca are cont
		//	error ca are deja
		//adaugam in bd
		//mesaj ca s-a adaugat
		/*auto [message, size] = JsonMessageHandler::MakeJson("", std::make_pair("",));
		m_serverHandler->SendData(message, size, clientId);*/
	}},

	{"createLobby", [this](void* data, int clientId)
	{
		auto lobbyId = GetLobbyIdFromClient(clientId);

		if (lobbyId != -1)
		{
			m_lobbies[lobbyId]->RemoveClient(clientId);

			if (m_lobbies[lobbyId]->GetNumberOfPlayers() == 0)
				RemoveLobby(GetLobbyFromIndex(lobbyId));
		}

		auto lobby = std::make_shared<Lobby>(m_lastLobbyId, m_serverHandler);
		m_lobbies.push_back(lobby);
		GetLobbyFromIndex(m_lastLobbyId)->AddClient(clientId);

		auto [message, size] = JsonMessageHandler::MakeJson("createdLobby", std::make_pair("createdLobbyId", m_lastLobbyId));
		m_serverHandler->SendData(message, size, clientId);

		++m_lastLobbyId;
	}},

	{"startGame", [this](void* data, int clientId)
	{
		auto lobbyId = GetLobbyIdFromClient(clientId);

		int numberOfPlayers = JsonMessageHandler::GetFromParam<int>("numberOfPlayers", data);

		if (lobbyId == -1)
		{
			auto [message, size] = JsonMessageHandler::MakeJson("error",
				std::make_pair("errorMessage", "Create a lobby first"));

			m_serverHandler->SendData(message, size, clientId);
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

	{"joinLobby", [this](void* data, int clientId)
	{
		auto lobbyId = JsonMessageHandler::GetFromParam<int>("lobbyId", data);

		if (GetLobbyIdFromClient(clientId) != -1)
		{
			m_lobbies[GetLobbyIdFromClient(clientId)]->RemoveClient(clientId);

			if (m_lobbies[lobbyId]->GetNumberOfPlayers() == 0)
				RemoveLobby(GetLobbyFromIndex(lobbyId));
		}

		bool foundLobby = false;
		for (auto& lobby : m_lobbies)
		{
			if (lobby->GetLobbyId() == lobbyId)
			{
				foundLobby = true;
				break;
			}
		}

		if (foundLobby)
		{
			m_lobbies[lobbyId]->AddClient(clientId);

			auto [message, size] = JsonMessageHandler::MakeJson("joinedLobby"
				, std::make_pair("lobbyId", lobbyId));

			m_serverHandler->SendData(message, size, clientId);
		}
		else
		{
			auto [message, size] = JsonMessageHandler::MakeJson("error"
				, std::make_pair("errorMessage", "Lobby inexistent"));

			m_serverHandler->SendData(message, size, clientId);
		}
	}},

	{"getActivePlayer", [this](void* data, int clientId)
	{
		int lobbyId = GetLobbyIdFromClient(clientId);
		auto game = m_lobbies[lobbyId]->GetGame();

		auto player = game->GetActivePlayer();

		auto [message, size] = JsonMessageHandler::MakeJson("playerChanged"
			, std::make_pair("player", EPieceTypeStringMaker::GetStringFromEnum(player))
			, std::make_pair("computer", game->IsComputerTurn()));

		m_serverHandler->SendData(message, size, clientId);
	}},

	{"setupBoard", [this](void* data, int clientId)
	{
		int lobbyId = GetLobbyIdFromClient(clientId);
		auto game = m_lobbies[lobbyId]->GetGame();

		auto [message, size] = JsonMessageHandler::BuildNodesVector(game->GetAllNodes());

		m_serverHandler->SendData(message, size, clientId);
	}},

	{"getPossibleMoves", [this](void* data, int clientId)
	{
		int lobbyId = GetLobbyIdFromClient(clientId);
		auto game = m_lobbies[lobbyId]->GetGame();

		auto indexes = game->GetPossibleMovesFromNode(JsonMessageHandler::GetFromParam<int>("fromIndex", data));

		auto [message, size] = JsonMessageHandler::MakeJson("possibleMoves"
			, std::make_pair("indexes", indexes));

		m_serverHandler->SendData(message, size, clientId);
	}},

	{"getPossibleRemoves", [this](void* data, int clientId)
	{
		int lobbyId = GetLobbyIdFromClient(clientId);
		auto game = m_lobbies[lobbyId]->GetGame();

		auto indexes = game->GetPossibleRemoves();

		auto [message, size] = JsonMessageHandler::MakeJson("possibleRemoves"
			, std::make_pair("indexes", indexes));

		m_serverHandler->SendData(message, size, clientId);
	}},

	{"getTimer", [this](void* data, int clientId)
	{
		int lobbyId = GetLobbyIdFromClient(clientId);
		if (lobbyId == -1)
			return;

		auto game = m_lobbies[lobbyId]->GetGame();

		double time = game->GetRoundTime();

		auto [message, size] = JsonMessageHandler::MakeJson("getTimer"
			, std::make_pair("time", static_cast<int>(time)));

		m_serverHandler->SendData(message, size, clientId);
	}},

	{"getWinningPlayer", [this](void* data, int clientId)
	{
		int lobbyId = GetLobbyIdFromClient(clientId);
		auto game = m_lobbies[lobbyId]->GetGame();

		auto winningPlayer = game->GetWinningPlayer();

		auto [message, size] = JsonMessageHandler::MakeJson("winningPlayer"
			, std::make_pair("player", EPieceTypeStringMaker::GetStringFromEnum(winningPlayer)));

		m_serverHandler->SendData(message, size, clientId);
	}},

	{"movePiece", [this](void* data, int clientId)
	{
		int lobbyId = GetLobbyIdFromClient(clientId);
		auto game = m_lobbies[lobbyId]->GetGame();

		if (!CheckPlayerTurn(lobbyId, clientId, game))
			return;

		auto fromIndex = JsonMessageHandler::GetFromParam<int>("fromIndex", data);
		auto toIndex = JsonMessageHandler::GetFromParam<int>("toIndex", data);

		game->MovePiece(fromIndex, toIndex);
	}},

	{"placePiece", [this](void* data, int clientId)
	{
		int lobbyId = GetLobbyIdFromClient(clientId);
		auto game = m_lobbies[lobbyId]->GetGame();

		if (!CheckPlayerTurn(lobbyId, clientId, game))
			return;

		auto index = JsonMessageHandler::GetFromParam<int>("index", data);

		game->PlacePiece(index);
	}},

	{"removePiece", [this](void* data, int clientId)
	{
		int lobbyId = GetLobbyIdFromClient(clientId);
		auto game = m_lobbies[lobbyId]->GetGame();

		if (!CheckPlayerTurn(lobbyId, clientId, game))
			return;

		auto index = JsonMessageHandler::GetFromParam<int>("index", data);

		game->RemovePiece(index);
	}},

	{"undoAction", [this](void* data, int clientId)
	{
		auto game = GetGame(clientId);

		game->Undo();
	}},

	{ "changedConfig", [this](void* data, int clientId)
	{
		int lobbyId = GetLobbyIdFromClient(clientId);

		if (lobbyId == -1)
		{
			auto [message, size] = JsonMessageHandler::MakeJson("error"
				, std::make_pair("errorMessage", "Not in a lobby")
			);

			m_serverHandler->SendData(message, size, clientId);
			return;
		}

		m_lobbies[lobbyId]->NotifyAllClients(data, static_cast<int>(std::strlen((char*)data)));
	}}
	};
}

bool MainServer::CheckPlayerTurn(int lobbyId, int clientId, IGamePtr game)
{
	if (m_lobbies[lobbyId]->GetClientIndexInLobby(clientId) != game->GetActivePlayerIndex())
	{
		auto [message, size] = JsonMessageHandler::MakeJson("error"
			, std::make_pair("errorMessage", "Not your turn")
		);

		m_serverHandler->SendData(message, size, clientId);
		return false;
	}

	return true;
}

LobbySharedPtr MainServer::GetLobbyFromIndex(int lobbyId)
{
	for (const auto& lobby : m_lobbies)
	{
		if (lobby->GetLobbyId() == lobbyId)
			return lobby;
	}
	return {};
}

int MainServer::GetLobbyIdFromClient(int clientId)
{
	for (auto it = m_lobbies.begin(); it != m_lobbies.end(); ++it)
	{
		if (it->get()->IsClientInLobby(clientId))
			return static_cast<int>(std::distance(m_lobbies.begin(), it));
	}

	return -1;
}

void MainServer::RemoveLobby(LobbySharedPtr lobby)
{
	for (auto it = m_lobbies.begin(); it != m_lobbies.end();)
	{
		if (*it == lobby)
		{
			it = m_lobbies.erase(it);
		}
		else
		{
			++it;
		}
	}
}

IGamePtr MainServer::GetGame(int clientId)
{
	int lobbyId = GetLobbyIdFromClient(clientId);
	return m_lobbies[lobbyId]->GetGame();
}

void MainServer::ConnectToDatabase()
{
	try
	{
		ReadDatabaseConfig();

		std::ostringstream connectionString;
		connectionString << "dbname=" << m_dbname
						<< " user=" << m_user
						<< " password=" << m_password
						<< " hostaddr=" << m_hostaddr
						<< " port=" << m_port;

		m_dbConnection = new pqxx::connection(connectionString.str());

		if (m_dbConnection->is_open())
			std::cerr << "Connected to database successfully: " << m_dbConnection->dbname() << std::endl;
		else
			std::cerr << "Failed to connect to database." << std::endl;
	}
	catch (const std::exception& e)
	{
		std::cerr << "Exception while connecting to database: " << e.what() << std::endl;
	}
}

void MainServer::ReadDatabaseConfig()
{
	std::ifstream file("DatabaseConnection.txt");

	std::string line;
	while (std::getline(file, line)) {
		size_t pos = line.find('=');
		if (pos == std::string::npos) continue;

		std::string key = line.substr(0, pos);
		std::string value = line.substr(pos + 1);

		if (key == "dbname") {
			m_dbname = value;
		}
		else if (key == "user") {
			m_user = value;
		}
		else if (key == "password") {
			m_password = value;
		}
		else if (key == "hostaddr") {
			m_hostaddr = value;
		}
		else if (key == "port") {
			m_port = std::stoi(value);
		}
	}

	file.close();
}
