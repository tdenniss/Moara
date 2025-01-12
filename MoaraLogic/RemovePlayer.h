#pragma once
#include "Command.h"
#include "EGameState.h"

using Indexes = std::vector<int>;

class Game;

class RemovePlayer : public Command
{
public:
	RemovePlayer(Game* game, EPlayerType player, bool isComputer, Miliseconds remainingTime, Indexes indexes);
	RemovePlayer(Game* game, PlayerPtr player);

	void Execute() override;
	void Undo() override;

	void Print(std::ostream& os) const override;
	void Read(std::istream& is) override;

private:
	EPlayerType m_player;
	bool m_isComputer;
	EGameState m_lastState;
	Miliseconds m_remainingTime;
	Indexes m_piecesIndexes;
};