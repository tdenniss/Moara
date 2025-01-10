#pragma once
#include "IPlayer.h"

#include <cstdint>

class Player : public IPlayer
{
public:
	Player();
	Player(EPlayerType type);
	~Player() = default;

	EPlayerType GetType() const override;
	void SetType(EPlayerType type) override;

private:
	EPlayerType m_type;
};