#include "pch.h"
#include "Player.h"

Player::Player()
	: m_type{ EPlayerType::None }
{
}

Player::Player(EPlayerType type)
	: m_type{ type }
{
}

EPlayerType Player::GetType() const
{
	return m_type;
}

void Player::SetType(EPlayerType type)
{
	m_type = type;
}
