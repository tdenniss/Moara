#include "pch.h"
#include "Move.h"

#include "Game.h"

Move::Move(Game* game, int from, int to)
	: Command(game)
	, m_from(from)
	, m_to(to)
{
}

void Move::Execute()
{
	if (m_game->m_state != EGameState::Moving)
	{
		return;
	}

	m_board->MovePiece(m_from, m_to, m_game->m_players[m_game->m_activePlayer]->GetType());

	m_game->NotifyAll(m_game->GetNotifyMovedPiece(m_from, m_to, m_game->m_players[m_game->m_activePlayer]->GetType()));

	m_game->CheckWiningPlayer();
	if (m_game->m_winner != EPlayerType::None)
		return;

	if (m_board->GetBoardState() == EBoardState::Full_line)
	{
		m_game->m_state = EGameState::Removing;
		m_game->NotifyAll(m_game->GetNotifyGameStateChanged(m_game->m_state));

		return;
	}
	else if (m_board->GetBoardState() == EBoardState::Windmill)
	{
		m_game->m_state = EGameState::Removing;
		m_game->NotifyAll(m_game->GetNotifyWindmill());

		return;
	}

	m_game->NextPlayer();
}

void Move::Undo()
{
	if (m_board->GetNodeType(m_to) != m_game->m_players[m_game->m_activePlayer]->GetType())
		m_game->PrevPlayer();

	m_board->UndoMovePiece(m_from, m_to);
	m_game->NotifyAll(m_game->GetNotifyMovedPiece(m_to, m_from, m_game->m_players[m_game->m_activePlayer]->GetType()));

	m_game->m_state = EGameState::Moving;
	m_game->NotifyAll(m_game->GetNotifyGameStateChanged(m_game->m_state));
}

void Move::Print(std::ostream& os) const
{
	os << static_cast<int>(CommandType::Move) << ' ';
	os << m_from << m_to << ' ';
	os << '\n';
}

void Move::Read(std::istream& is)
{
	is >> m_from >> m_to;
}
