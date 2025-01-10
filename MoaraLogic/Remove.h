#pragma once
#include "Command.h"

class Game;

class Remove : public Command
{
public:
	Remove(Game* game, int index);

	void Execute() override;
	void Undo() override;

	void Print(std::ostream& os) const override;
	void Read(std::istream& is) override;

private:
	int m_index;
};