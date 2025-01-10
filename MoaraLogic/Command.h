#pragma once
#include "IBoard.h"

#include <ostream>
#include <memory>

class Game;

using CommandPtr = std::shared_ptr<class Command>;

class Command
{
public:
	enum class CommandType
	{
		Place,
		Remove,
		Move,
		RemovePlayer
	};

	static CommandPtr Create(Game* game, std::vector<int> values);

	Command(Game* game);

	virtual void Execute() = 0;
	virtual void Undo() = 0;

	virtual void Print(std::ostream& os) const = 0;
	virtual void Read(std::istream& is) = 0;

	virtual ~Command() = default;

protected:
	Game* m_game;
	IBoardPtr m_board;
};
