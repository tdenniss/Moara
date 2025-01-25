#include "SDK.h"

#include "JsonMessageHandler.h"
#include "EComputerLevel.h"

SDK::SDK(std::string ip, int port, IServerHandlerPtr serverHandler)
	: m_serverHandler(serverHandler)
	, m_listener(nullptr)
{
	InitializeCommands();

	m_serverHandler->SetNewDataCallback(std::bind(&SDK::OnNewData, this, std::placeholders::_1));
	m_serverHandler->SetDisconnectedCallback(std::bind(&SDK::OnServerDisconnected, this));
	m_serverHandler->Start(ip, port);
}

SDK::~SDK()
{
	m_serverHandler->Stop();
}

void SDK::SetListener(IClientSDKListener* listener)
{
	m_listener = listener;
}

void SDK::PlacePiece(uint8_t index)
{
	auto [message, size] = JsonMessageHandler::MakeJson("placePiece"
		, std::make_pair("index", index));

	m_serverHandler->SendRequest(message, size);
}

void SDK::RemovePiece(uint8_t index)
{
	auto [message, size] = JsonMessageHandler::MakeJson("removePiece"
		, std::make_pair("index", index));

	m_serverHandler->SendRequest(message, size);
}

void SDK::MovePiece(uint8_t from, uint8_t to)
{
	auto [message, size] = JsonMessageHandler::MakeJson("movePiece"
		, std::make_pair("fromIndex", from)
		, std::make_pair("toIndex", to));

	m_serverHandler->SendRequest(message, size);
}

void SDK::StartGame(GameConfigPtr config)
{
	auto [message, size] = JsonMessageHandler::MakeGameConfigJson(config);

	m_serverHandler->SendRequest(message, size);
}

void SDK::SetupBoard()
{
	auto [message, size] = JsonMessageHandler::MakeJson("setupBoard");

	m_serverHandler->SendRequest(message, size);
}

void SDK::CreateLobby()
{
	auto [message, size] = JsonMessageHandler::MakeJson("createLobby");

	m_serverHandler->SendRequest(message, size);
}

void SDK::JoinLobby(int id)
{
	auto [message, size] = JsonMessageHandler::MakeJson("joinLobby"
		, std::make_pair("lobbyId", id));

	m_serverHandler->SendRequest(message, size);
}

void SDK::ChangeConfig(int boardSize, int boardType, EComputerLevel level)
{
	auto [message, size] = JsonMessageHandler::MakeJson("changedConfig"
		, std::make_pair("boardSize", boardSize)
		, std::make_pair("boardType", boardType)
		, std::make_pair("level", level));

	m_serverHandler->SendRequest(message, size);
}

void SDK::Undo()
{
	auto [message, size] = JsonMessageHandler::MakeJson("undoAction");

	m_serverHandler->SendRequest(message, size);
}

void SDK::GetTimer()
{
	auto [message, size] = JsonMessageHandler::MakeJson("getTimer");

	m_serverHandler->SendRequest(message, size);
}

void SDK::GetPossibleMoves(uint8_t index)
{
	auto [message, size] = JsonMessageHandler::MakeJson("getPossibleMoves"
		, std::make_pair("fromIndex", index));

	m_serverHandler->SendRequest(message, size);
}

void SDK::GetPossibleRemoves()
{
	auto [message, size] = JsonMessageHandler::MakeJson("getPossibleRemoves");

	m_serverHandler->SendRequest(message, size);
}

void SDK::GetWinnigPlayer()
{
	auto [message, size] = JsonMessageHandler::MakeJson("getWinningPlayer");

	m_serverHandler->SendRequest(message, size);
}

void SDK::GetActivePlayer()
{
	auto [message, size] = JsonMessageHandler::MakeJson("getActivePlayer");

	m_serverHandler->SendRequest(message, size);
}

void SDK::SignUp(std::string username, std::string password)
{
	auto [message, size] = JsonMessageHandler::MakeJson("signUp"
		, std::make_pair("username", username)
		, std::make_pair("password", password));

	m_serverHandler->SendRequest(message, size);
}

void SDK::Login(std::string username, std::string password)
{
	auto [message, size] = JsonMessageHandler::MakeJson("login"
		, std::make_pair("username", username)
		, std::make_pair("password", password));

	m_serverHandler->SendRequest(message, size);
}

void SDK::OnNewData(void* data)
{
	Json notification = Json::parse((char*)data);

	std::string notificationType = notification["action"].get<std::string>();

	auto callbackIt = m_commands.find(notificationType);
	if (callbackIt != m_commands.end())
	{
		callbackIt->second(data);
	}
}

void SDK::OnServerDisconnected()
{
	m_listener->OnServerDisconnect();
}

void SDK::InitializeCommands()
{
	m_commands = {
	{"signUpSuccess", [this](void* data)
	{
		m_listener->OnSignUpSuccess();
	}},

	{"loginSuccess", [this](void* data)
	{
		m_listener->OnLoginSuccess();
	}},

	{"setupBoard", [this](void* data)
	{
		m_listener->OnSetupBoard(JsonMessageHandler::GetNodesFromJson(data));
	}},

	{"getTimer", [this](void* data)
	{
		m_listener->OnUpdateTimer(JsonMessageHandler::GetFromParam<int>("time", data));
	}},

	{"winningPlayer", [this](void* data)
	{
		m_listener->OnWinningPlayer(JsonMessageHandler::GetFromParam<std::string>("player", data));
	}},

	{"addedPiece", [this](void* data)
	{
		int index = JsonMessageHandler::GetFromParam<int>("index", data);
		EPieceType pieceType = EPieceTypeStringMaker::GetEnumFromString(JsonMessageHandler::GetFromParam<std::string>("nodeType", data));

		m_listener->OnAddedPiece(index, pieceType);
	}},

	{"removedPiece", [this](void* data)
	{
		int index = JsonMessageHandler::GetFromParam<int>("index", data);

		m_listener->OnRemovedPiece(index);
	}},

	{"movedPiece", [this](void* data)
	{
		int from = JsonMessageHandler::GetFromParam<int>("fromNode", data);
		int to = JsonMessageHandler::GetFromParam<int>("toNode", data);

		EPieceType pieceType = EPieceTypeStringMaker::GetEnumFromString(JsonMessageHandler::GetFromParam<std::string>("nodeType", data));

		m_listener->OnMovedPiece(from, to, pieceType);
	}},

	{"playerRemoved", [this](void* data)
	{
		std::string player = JsonMessageHandler::GetFromParam<std::string>("who", data);

		m_listener->OnPlayerRemoved(player);
	}},

	{"playerChanged", [this](void* data)
	{
		std::string player = JsonMessageHandler::GetFromParam<std::string>("who", data);
		bool isComputer = JsonMessageHandler::GetFromParam<bool>("computer", data);

		m_listener->OnPlayerChanged(player, isComputer);
	}},

	{"stateChanged", [this](void* data)
	{
		EGameState gameState = EGameStateStringMaker::GetEnumFromString(JsonMessageHandler::GetFromParam<std::string>("newState", data));

		m_listener->OnStateChanged(gameState);
	}},

	{"windmill", [this](void* data)
	{
		m_listener->OnWindmill();
	}},

	{"possibleRemoves", [this](void* data)
	{
		m_listener->OnPossibleRemoves(JsonMessageHandler::GetFromParam<PiecesIndexes>("indexes", data));
	}},

	{"possibleMoves", [this](void* data)
	{
		m_listener->OnPossibleMoves(JsonMessageHandler::GetFromParam<PiecesIndexes>("indexes", data));
	}},

	{"error", [this](void* data)
	{
		m_listener->OnError(JsonMessageHandler::GetFromParam<std::string>("errorMessage", data));
	}},

	{"info", [this](void* data)
	{
		m_listener->OnInfo(JsonMessageHandler::GetFromParam<std::string>("infoMessage", data));
	}},

	{"playerLeft", [this](void* data)
	{
		m_listener->OnPlayerLeft();
	}},

	{"gameStarted", [this](void* data)
	{
		m_listener->OnGameStarted();
	}},

	{"joinedLobby", [this](void* data)
	{
		m_listener->OnJoinedLobby(JsonMessageHandler::GetFromParam<int>("lobbyId", data));
	}},

	{"createdLobby", [this](void* data)
	{
		m_listener->OnCreatedLobby(JsonMessageHandler::GetFromParam<int>("createdLobbyId", data));
	}},

	{"changedConfig", [this](void* data)
	{
		auto type = JsonMessageHandler::GetFromParam<int>("boardType", data);
		auto size = JsonMessageHandler::GetFromParam<int>("boardSize", data);
		auto levelInt = JsonMessageHandler::GetFromParam<int>("level", data);
		auto level = (EComputerLevel)levelInt;
		m_listener->OnChangedConfig(static_cast<EBoardType>(type), static_cast<EBoardSize>(size), level);
	}},
	};
}
