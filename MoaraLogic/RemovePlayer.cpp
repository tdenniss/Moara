#include "pch.h"
#include "RemovePlayer.h"
#include "Game.h"
#include "Player.h"

RemovePlayer::RemovePlayer(Game* game, EPlayerType player, bool isComputer, Indexes indexes)
	: Command(game)
	, m_player(player)
	, m_lastState(game->m_state)
	, m_piecesIndexes(indexes)
{
}

RemovePlayer::RemovePlayer(Game* game, PlayerPtr player)
	: Command(game)
	, m_player(player->GetType())
	, m_lastState(game->m_state)
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

			m_game->NotifyAll(m_game->GetNotifyGameStateChanged(m_game->m_state));
		}
		else
		{
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
}

void RemovePlayer::Print(std::ostream& os) const
{
	os << static_cast<int>(CommandType::RemovePlayer) << ' ';
	os << static_cast<int>(m_player) << ' ';
	os << static_cast<int>(m_lastState) << ' ';
	for (const auto& index : m_piecesIndexes)
		os << index << ' ';
	os << '\n';
}

void RemovePlayer::Read(std::istream& is)
{
	//todo
}