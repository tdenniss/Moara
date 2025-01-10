#include "pch.h"
#include "Remove.h"
#include "Game.h"

Remove::Remove(Game* game, int index)
	: Command(game)
	, m_index(index)
{
}

void Remove::Execute()
{
	if (m_game->m_state != EGameState::Removing)
	{
		return;
	}

	m_board->RemovePiece(m_index, m_game->m_players[m_game->m_activePlayer]->GetType());

	m_game->NotifyAll(m_game->GetNotifyRemovedPiece(m_index));

	m_game->CheckWiningPlayer();
	if (m_game->m_winner != EPlayerType::None)
		return;

	m_game->m_state = EGameState::Moving;
	for (const auto& player : m_game->m_players)
	{
		if (m_board->GetPlayerPiecesToPlace(player->GetType()) > 0)
		{
			m_game->m_state = EGameState::Placing;
			break;
		}
	}

	m_game->NotifyAll(m_game->GetNotifyGameStateChanged(m_game->m_state));

	m_game->NextPlayer();
}

void Remove::Undo()
{
	m_board->UndoRemovePiece(m_index, m_game->m_players[m_game->m_activePlayer]->GetType());

	auto nodeType = m_game->m_players[m_game->m_activePlayer]->GetType();
	m_game->NotifyAll(m_game->GetNotifyAddedPiece(m_index, nodeType));

	m_game->m_state = EGameState::Removing;
	m_game->NotifyAll(m_game->GetNotifyGameStateChanged(m_game->m_state));

	m_game->PrevPlayer();
}

void Remove::Print(std::ostream& os) const
{
	os << static_cast<int>(CommandType::Remove) << ' ';
	os << m_index << ' ';
	os << '\n';
}

void Remove::Read(std::istream& is)
{
	is >> m_index;
}
