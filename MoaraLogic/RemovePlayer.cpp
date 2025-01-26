#include "pch.h"

#include "RemovePlayer.h"

#include "Game.h"
#include "Player.h"

RemovePlayer::RemovePlayer(Game* game, EPlayerType player, bool isComputer, Miliseconds remainingTime, Indexes indexes)
	: Command(game)
	, m_player(player)
	, m_isComputer(isComputer)
	, m_lastState(game->m_state)
	, m_remainingTime(remainingTime)
	, m_piecesIndexes(indexes)
{}

RemovePlayer::RemovePlayer(Game* game, PlayerPtr player)
	: Command(game)
	, m_player(player->GetType())
	, m_isComputer(player->IsComputer())
	, m_lastState(game->m_state)
	, m_remainingTime(player->GetTimerRemainingDuration())
	, m_piecesIndexes()
{
	for (const auto& node : m_board->GetAllNodes())
	{
		if (node->GetPieceType() == m_player)
			m_piecesIndexes.push_back(node->GetIndex());
	}
}

void RemovePlayer::Execute()
{
	if (m_game->m_state == EGameState::Finished)
		return;

	EPlayerType playerType = m_player;

	auto it = std::find_if(m_game->m_players.begin(), m_game->m_players.end(),
		[playerType](const auto& player) { return player->GetType() == playerType; });

	if (it != m_game->m_players.end())
	{
		(*it).~shared_ptr();
		m_game->m_players.erase(it);

		m_game->m_activePlayer %= m_game->m_players.size();

		m_board->RemovePlayerPieces(playerType);

		m_game->NotifyAll(m_game->GetNotifyPlayerRemoved(playerType));

		if (m_game->m_players.size() == 1)
		{
			m_game->m_winner = m_game->m_players[0]->GetType();
			m_game->m_state = EGameState::Finished;

			m_game->m_moveTimer->Stop();

			m_game->NotifyAll(m_game->GetNotifyGameStateChanged(m_game->m_state));
		}
		else
		{
			m_game->m_players[m_game->m_activePlayer]->StartTimer();

			m_game->ResetMoveTimer();

			m_game->NotifyAll(m_game->GetNotifyPlayerChanged(m_game->GetActivePlayer(), m_game->m_players[m_game->m_activePlayer]->IsComputer()));
		}
	}
}

void RemovePlayer::Undo()
{
	m_game->m_state = m_lastState;

	PlayerPtr player = std::make_shared<Player>(m_player, m_isComputer);

	if (m_game->m_activePlayer == 0)
		m_game->m_players.push_back(player);
	else
		m_game->m_players.insert(m_game->m_players.begin() + m_game->m_activePlayer, player);

	m_board->SetPlayerPiecesToPlace(m_player, static_cast<int>(m_piecesIndexes.size()));
	m_board->SetPlayerPiecesOnTable(m_player, 0);

	for (const auto& index : m_piecesIndexes)
	{
		m_board->AddPiece(index, m_player);
	}

	if (player->IsComputer())
	{
		player->SetTimerDuration(Game::RandomMiliseconds(Miliseconds(1000), Miliseconds(5000)));//random between 1-5sec

		player->SetTimerCallback([&]()
			{
				m_game->LetComputerPlay();

				player->SetTimerDuration(Game::RandomMiliseconds(Miliseconds(1000), Miliseconds(5000)));//random between 1-5sec
			});
	}
	else
	{
		player->SetTimerDuration(m_remainingTime);

		player->SetTimerCallback([this, player]()
			{
				m_game->m_moves.push_back(std::make_shared<RemovePlayer>(RemovePlayer(m_game, player)));

				m_game->m_moves.back()->Execute();
			});
	}

	m_game->ResetMoveTimer();
}

void RemovePlayer::Print(std::ostream& os) const
{
	os << static_cast<int>(CommandType::RemovePlayer) << ' ';
	os << static_cast<int>(m_player) << ' ';
	os << static_cast<int>(m_isComputer) << ' ';
	os << static_cast<int>(m_lastState) << ' ';
	os << m_remainingTime.count() << ' ';
	for (const auto& index : m_piecesIndexes)
		os << index << ' ';
	os << '\n';
}

void RemovePlayer::Read(std::istream& is)
{
	//todo
}