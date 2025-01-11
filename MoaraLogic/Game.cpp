#include "pch.h"

#include "Game.h"
#include "Player.h"
#include "FileManager.h"
#include "IGameListener.h"
#include "MediumComputerLevel.h"
#include "HardComputerLevel.h"
#include "EasyComputerLevel.h"
#include "Place.h"
#include "Move.h"
#include "Remove.h"
#include "RemovePlayer.h"

#include <algorithm>
#include <memory>

IGamePtr IGame::Produce(GameConfigPtr& gameConfig)
{
	auto game = std::make_shared<Game>();
	try
	{
		game->Initialize(gameConfig);
	}
	catch (...)
	{
		return {};
	}
	return game;
}

Game::Game()
	: m_activePlayer{ 0 }
	, m_state{ EGameState::Placing }
	, m_winner{ EPlayerType::None }
	, m_players{}
{}

void Game::Initialize(GameConfigPtr& gameConfig)
{
	InitLevel(gameConfig->GetComputerLevel());

	InitPlayers(gameConfig->GetPlayerConfig());

	InitBoard(gameConfig->GetBoardConfigMatrix(), gameConfig->GetBoardType(), gameConfig->GetNumberOfPiecesToPlace());
}

void Game::PlacePiece(uint8_t nodeIndex)
{
	m_moves.push_back(std::make_shared<Place>(Place(this, nodeIndex)));

	m_moves.back()->Execute();
}

void Game::MovePiece(uint8_t fromIndex, uint8_t toIndex)
{
	m_moves.push_back(std::make_shared<Move>(Move(this, fromIndex, toIndex)));

	m_moves.back()->Execute();
}

void Game::RemovePiece(uint8_t nodeIndex)
{
	m_moves.push_back(std::make_shared<Remove>(Remove(this, nodeIndex)));

	m_moves.back()->Execute();
}

void Game::Undo()
{
	auto& move = m_moves.back();

	move->Undo();

	m_moves.pop_back();
}

void Game::LoadGame(const std::string& filePath)
{
	std::ifstream file(filePath);

	if (!file.is_open())
	{
		return;
	}

	std::string line;

	auto ReadNextInts = [&](std::vector<int>& values)
		{
			values.clear();
			while (std::getline(file, line))
			{
				if (line.empty() || line[0] == '#')
				{
					continue;
				}
				std::istringstream iss(line);
				int value;
				while (iss >> value)
				{
					values.push_back(value);
				}
				if (!values.empty())
				{
					break;
				}
			}
		};

	std::vector<int> values;

	//active player
	ReadNextInts(values);
	if (!values.empty()) {
		m_activePlayer = values[0];
	}

	//game state
	ReadNextInts(values);
	if (!values.empty()) {
		m_state = static_cast<EGameState>(values[0]);
	}

	PieceTypeList piecesTypes(0);
	for (const auto& player : m_players)
	{
		piecesTypes.push_back(player->GetType());
	}

	//board type
	ReadNextInts(values);
	if (!values.empty())
	{
		int type = values[0];
		EBoardType boardType = static_cast<EBoardType>(type);
		m_board = BoardFactory::Create(boardType, piecesTypes, file);
	}

	//moves number
	ReadNextInts(values);
	int movesNumber = values.empty() ? 0 : values[0];

	//moves
	for (uint8_t index = 0; index < movesNumber; ++index)
	{
		ReadNextInts(values);
		if (values.empty()) continue;

		m_moves.push_back(Command::Create(this, values));
	}

	file.close();
}

void Game::SaveGame(const std::string& filePath) const
{
	std::ofstream file(filePath, std::ios_base::out);

	if (!file.is_open())
	{
		return;
	}

	file << "#active player" << std::endl;
	file << m_activePlayer << std::endl;

	file << "#last state" << std::endl;
	file << static_cast<int>(m_state) << std::endl;

	file << "#board type" << std::endl;
	file << static_cast<int>(m_board->GetBoardType()) << std::endl;

	m_board->SaveBoard(file);

	file << "#moves count" << std::endl;

	file << m_moves.size() << std::endl;

	file << "#moves" << std::endl;

	for (const auto& move : m_moves)
	{
		file << move;
	}

	file.close();
}

void Game::AddListener(ListenerSharedPtr listener)
{
	m_listeners.push_back(listener);
}

void Game::SetBoard(IBoardPtr board)
{
	m_board = board;
}

void Game::SetComputerLevel(EComputerLevel level)
{
	InitLevel(level);
}

void Game::SetComputerLevel(ComputerLevelPtr computerLevel)
{
	m_computer = computerLevel;
}

EPieceType Game::GetNode(uint8_t nodeIndex) const
{
	return m_board->GetNodeType(nodeIndex);
}

NodeList Game::GetAllNodes() const
{
	return m_board->GetAllNodes();
}

void Game::NextPlayer()
{
	m_activePlayer = ++m_activePlayer % m_players.size();

	NotifyAll(GetNotifyPlayerChanged(GetActivePlayer(), m_players[m_activePlayer]->IsComputer()));
}

void Game::PrevPlayer()
{
	m_activePlayer = --m_activePlayer % m_players.size();

	NotifyAll(GetNotifyPlayerChanged(GetActivePlayer(), m_players[m_activePlayer]->IsComputer()));
}

void Game::LetComputerPlay()
{
	if (m_players[m_activePlayer]->IsComputer() == true)
	{
		EPieceType computerPieceType = m_players[m_activePlayer]->GetType();

		uint8_t whereToPlace;
		MovePosition whereToMove;
		uint8_t whereToRemove;

		switch (m_state)
		{
		case EGameState::Placing:
			whereToPlace = m_computer->GetPlacingIndex(m_board, computerPieceType);

			PlacePiece(whereToPlace);
			break;
		case EGameState::Moving:
			whereToMove = m_computer->GetMovingIndex(m_board, computerPieceType);

			MovePiece(whereToMove.first, whereToMove.second);
			break;
		default:
			break;
		}

		if (m_state == EGameState::Removing)
		{
			whereToRemove = m_computer->GetRemovingIndex(m_board, computerPieceType);
			RemovePiece(whereToRemove);
		}
	}
}

bool Game::IsComputerTurn()
{
	return m_players[m_activePlayer]->IsComputer();
}

size_t Game::HowManyListeners()
{
	return m_listeners.size();
}

void Game::SetGameState(EGameState state)
{
	m_state = state;
}

int Game::GetActivePlayerIndex() const
{
	return m_activePlayer;
}

void Game::SetPlayerConfig(PlayerConfig& config)
{
	InitPlayers(config.GetConfig());
}

void Game::RemoveListener(ListenerWeakPtr listener)
{
	auto listenerPtr = listener.lock();
	if (!listenerPtr) return;

	auto it = std::ranges::remove_if(m_listeners,
		[&listenerPtr](const ListenerWeakPtr& listener) {
			auto elemPtr = listener.lock();
			return elemPtr && elemPtr == listenerPtr;
		});

	m_listeners.erase(it.begin(), it.end());
}

void Game::RemoveAllListeners()
{
	m_listeners.clear();
}

void Game::NotifyAll(NotifyFunction func)
{
	for (auto listener : m_listeners)
	{
		if (auto sp = listener.lock())
		{
			func(sp.get());
		}
	}
}

void Game::UndoPlacePiece(uint8_t nodeIndex)
{
	if (m_state != EGameState::Removing)
		PrevPlayer();

	m_board->UndoAddPiece(nodeIndex);
	NotifyAll(GetNotifyRemovedPiece(nodeIndex));

	m_state = EGameState::Placing;
	NotifyAll(GetNotifyGameStateChanged(m_state));
}

void Game::UndoMovePiece(uint8_t fromIndex, uint8_t toIndex)
{
	if (m_board->GetNodeType(toIndex) != m_players[m_activePlayer]->GetType())
		PrevPlayer();

	m_board->UndoMovePiece(fromIndex, toIndex);
	NotifyAll(GetNotifyMovedPiece(toIndex, fromIndex, m_players[m_activePlayer]->GetType()));

	m_state = EGameState::Moving;
	NotifyAll(GetNotifyGameStateChanged(m_state));
}

void Game::UndoRemovePiece(uint8_t nodeIndex)
{
	m_board->UndoRemovePiece(nodeIndex, m_players[m_activePlayer]->GetType());

	auto nodeType = m_players[m_activePlayer]->GetType();
	NotifyAll(GetNotifyAddedPiece(nodeIndex, nodeType));

	m_state = EGameState::Removing;
	NotifyAll(GetNotifyGameStateChanged(m_state));

	PrevPlayer();
}

NotifyFunction Game::GetNotifyAddedPiece(uint8_t nodeIndex, EPieceType nodeType)
{
	return [&nodeIndex, &nodeType](IGameListener* listener) {
		listener->OnAddedPiece(nodeIndex, nodeType);
		};
}

NotifyFunction Game::GetNotifyGameStateChanged(EGameState newState)
{
	return [&](IGameListener* listener) {
		listener->OnGameStateChanged(m_state);
		};
}

NotifyFunction Game::GetNotifyRemovedPiece(uint8_t index)
{
	return [&](IGameListener* listener) {
		listener->OnRemovedPiece(index);
		};
}

NotifyFunction Game::GetNotifyMovedPiece(uint8_t fromIndex, uint8_t toIndex, EPieceType movedPiceType)
{
	return [&](IGameListener* listener) {
		listener->OnMoveMade(fromIndex, toIndex, movedPiceType);
		};
}

NotifyFunction Game::GetNotifyWindmill()
{
	return [&](IGameListener* listener) {
		listener->OnWindmillRule();
		};
}

NotifyFunction Game::GetNotifyPlayerChanged(EPieceType playerType, bool isComputer)
{
	return [&](IGameListener* listener) {
		listener->OnPlayerChanged(playerType, isComputer);
		};
}

NotifyFunction Game::GetNotifyPlayerRemoved(EPieceType who)
{
	return [&](IGameListener* listener) {
		listener->OnPlayerRemoved(who);
		};
}

void Game::InitLevel(EComputerLevel level)
{
	switch (level)
	{
	case EComputerLevel::None:
		m_computer = nullptr;
		break;
	case EComputerLevel::Easy:
		m_computer = std::make_shared<EasyComputerLevel>();
		break;
	case EComputerLevel::Medium:
		m_computer = std::make_shared<MediumComputerLevel>();
		break;
	case EComputerLevel::Hard:
		m_computer = std::make_shared<HardComputerLevel>();
	default:
		break;
	}
}

void Game::InitPlayers(const Config& playersConfig)
{
	m_players.clear();
	for (const auto& config : playersConfig)
	{
		auto [type, isComputer] = config;

		m_players.emplace_back(std::make_shared<Player>(static_cast<EPieceType>(type), isComputer));
	}
}

void Game::InitBoard(const BoardConfigMatrix& boardMatrix, EBoardType type, int piecesToPlace)
{
	PieceTypeList piecesTypes(0);
	for (const auto& player : m_players)
	{
		piecesTypes.push_back(player->GetType());
	}

	m_board = BoardFactory::Create(type, piecesTypes, boardMatrix, piecesToPlace);
}

EPieceType Game::GetPiece(uint8_t nodeIndex) const
{
	return m_board->GetNodeType(nodeIndex);
}

PieceTypeList Game::GetAllPieces() const
{
	return m_board->GetAllNodesType();
}

PlayerTypeList Game::GetAllPlayerTypes() const
{
	PlayerTypeList players;
	players.reserve(m_players.size());
	for (const auto& player : m_players)
		players.push_back(player->GetType());
	return players;
}

uint8_t Game::GetPlayerPiecesOnBoard(EPieceType player) const
{
	return m_board->GetPlayerPiecesOnTable(player);
}

uint8_t Game::GetPlayerPiecesToPlace(EPieceType player) const
{
	return m_board->GetPlayerPiecesToPlace(player);
}

PieceIndexes Game::GetPossibleMovesFromNode(uint8_t nodeIndex) const
{
	return m_board->GetPossibleMovesFromNode(nodeIndex, m_players[m_activePlayer]->GetType());
}

PieceIndexes Game::GetPossibleRemoves() const
{
	return m_board->GetPossibleRemoves(m_players[m_activePlayer]->GetType());
}

void Game::CheckBlocks()
{
	for (int index = 0; index < m_players.size();)
	{
		if (m_board->GetPlayerPiecesToPlace(m_players[index]->GetType()) == 0 &&
			m_board->GetPossibleMoves(m_players[index]->GetType()).size() == 0)
		{
			m_moves.push_back(std::make_shared<RemovePlayer>(RemovePlayer(this, m_players[index])));

			m_moves.back()->Execute();
		}
		else
			++index;
	}
}

void Game::CheckWiningPlayer()
{
	if (m_players.size() == 1)
	{
		m_winner = m_players[0]->GetType();
		m_state = EGameState::Finished;

		NotifyAll(GetNotifyGameStateChanged(m_state));

		return;
	}

	CheckBlocks();

	for (int index = 0; index < m_players.size();)
	{
		if (m_board->GetPlayerPiecesToPlace(m_players[index]->GetType()) == 0 &&
			m_board->GetPlayerPiecesOnTable(m_players[index]->GetType()) == 2)
		{
			m_moves.push_back(std::make_shared<RemovePlayer>(RemovePlayer(this, m_players[index])));

			m_moves.back()->Execute();
		}
		else
			++index;
	}
}

EGameState Game::GetGameState() const
{
	return m_state;
}

EPieceType Game::GetWinningPlayer() const
{
	return m_winner;
}

EPieceType Game::GetActivePlayer() const
{
	return m_players[m_activePlayer]->GetType();
}
