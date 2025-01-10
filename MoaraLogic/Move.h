#pragma once
#include "Command.h"

class Game;

class Move : public Command
{
public:
	Move(Game* game, int from, int to);

	void Execute() override;
	void Undo() override;

	void Print(std::ostream& os) const override;
	void Read(std::istream& is) override;

private:
	int m_from;
	int m_to;
};