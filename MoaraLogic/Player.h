#pragma once
#include "IPlayer.h"

#include <cstdint>

class Player : public IPlayer
{
public:
	Player();
	Player(EPlayerType type, bool isComputer);

	EPlayerType GetType() const override;
	void SetType(EPlayerType type) override;

	bool IsComputer() const override;
	void SetComputer(bool isComputer) override;

private:
	EPlayerType m_type;
	bool m_isComputer;
};