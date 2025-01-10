#pragma once
#include "Command.h"
#include "EGameState.h"

using Indexes = std::vector<int>;

class Game;

class RemovePlayer : public Command
{
public:
	RemovePlayer(Game* game, EPlayerType player, bool isComputer, Indexes indexes);
	RemovePlayer(Game* game, PlayerPtr player);

	void Execute() override;
	void Undo() override;

	void Print(std::ostream& os) const override;
	void Read(std::istream& is) override;

private:
	EPlayerType m_player;
	EGameState m_lastState;
	Indexes m_piecesIndexes;
};