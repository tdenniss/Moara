#include "pch.h"
#include "Command.h"

#include "Game.h"
#include "Place.h"
#include "Move.h"
#include "Remove.h"
#include "RemovePlayer.h"

CommandPtr Command::Create(Game* game, std::vector<int> values)
{
	int type = values[0];

	switch (static_cast<CommandType>(type))
	{
	case CommandType::Place:
		return std::make_shared<Place>(game, values[1]);
	case CommandType::Remove:
		return std::make_shared<Remove>(game, values[1]);
	case CommandType::Move:
		return std::make_shared<Move>(game, values[1], values[2]);
	case CommandType::RemovePlayer:
		return std::make_shared<RemovePlayer>(game, static_cast<EPlayerType>(values[1]),
			values[2], Indexes(values.begin() + 4, values.end()));
	default:
		return {};
	}
}

Command::Command(Game* game)
	: m_game(game)
	, m_board(game->m_board)
{
}

std::ostream& operator<<(std::ostream& os, const Command& move)
{
	move.Print(os);
	return os;
}

std::istream& operator>>(std::istream& is, Command& move)
{
	move.Read(is);
	return is;
}
