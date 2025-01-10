#include "pch.h"
#include "Place.h"
#include "Game.h"

Place::Place(Game* game, int index)
	: Command(game)
	, m_index(index)
{
}

void Place::Execute()
{
	if (m_game->m_state != EGameState::Placing)
	{
		return;
	}

	m_board->AddPiece(m_index, m_game->m_players[m_game->m_activePlayer]->GetType());

	auto nodeType = m_game->m_players[m_game->m_activePlayer]->GetType();

	m_game->NotifyAll(m_game->GetNotifyAddedPiece(m_index, nodeType));

	m_game->CheckWiningPlayer();
	if (m_game->m_winner != EPlayerType::None)// stop if we have a winner
		return;

	if (m_board->GetBoardState() == EBoardState::Full_line)
	{
		m_game->m_state = EGameState::Removing;
		m_game->NotifyAll(m_game->GetNotifyGameStateChanged(m_game->m_state));

		return;
	}

	m_game->m_state = EGameState::Moving;

	for (const auto& player : m_game->m_players)
	{
		if (m_board->GetPlayerPiecesToPlace(player->GetType()) != 0)
			m_game->m_state = EGameState::Placing;
	}

	if (m_game->m_state != EGameState::Placing)
		m_game->NotifyAll(m_game->GetNotifyGameStateChanged(m_game->m_state));

	m_game->NextPlayer();
}

void Place::Undo()
{
	if (m_game->m_state != EGameState::Removing)
		m_game->PrevPlayer();

	m_board->UndoAddPiece(m_index);
	m_game->NotifyAll(m_game->GetNotifyRemovedPiece(m_index));

	m_game->m_state = EGameState::Placing;
	m_game->NotifyAll(m_game->GetNotifyGameStateChanged(m_game->m_state));
}

void Place::Print(std::ostream& os) const
{
	os << static_cast<int>(CommandType::Place) << ' ';
	os << m_index << ' ';
	os << '\n';
}

void Place::Read(std::istream& is)
{
	is >> m_index;
}
