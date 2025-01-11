#include "pch.h"

#include "Player.h"

Player::Player() :
	m_type{ EPlayerType::None },
	m_isComputer{ false }
{}

Player::Player(EPlayerType type, bool isComputer) :
	m_type{ type },
	m_isComputer{ isComputer }
{}

EPlayerType Player::GetType() const
{
	return m_type;
}

void Player::SetType(EPlayerType type)
{
	m_type = type;
}

bool Player::IsComputer() const
{
	return m_isComputer;
}

void Player::SetComputer(bool isComputer)
{
	m_isComputer = isComputer;
}
