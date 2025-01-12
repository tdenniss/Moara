#include "pch.h"

#include "Player.h"

Player::Player() :
	m_type{ EPlayerType::None },
	m_isComputer{ false },
	m_playerTimer{ std::make_shared<Timer>(Miliseconds(600000)) }
{}

Player::Player(EPlayerType type, bool isComputer) :
	m_type{ type },
	m_isComputer{ isComputer },
	m_playerTimer{ std::make_shared<Timer>(Miliseconds(600000)) }
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

void Player::StartTimer()
{
	m_playerTimer->Start();
}

void Player::StopTimer()
{
	m_playerTimer->Stop();
}

void Player::PauseTimer()
{
	m_playerTimer->Pause();
}

void Player::ResetTimer()
{
	m_playerTimer->Reset();
}

void Player::SetTimerCallback(TimerExpiredCallback callback)
{
	m_playerTimer->SetExpiredCallback(callback);
}

void Player::SetTimerDuration(Miliseconds duration)
{
	m_playerTimer->SetDuration(duration);
}

void Player::AddTimerDuration(Miliseconds duration)
{
	m_playerTimer->AddDuration(duration);
}

void Player::SubtractTimerDuration(Miliseconds duration)
{
	m_playerTimer->SubtractDuration(duration);
}

Miliseconds Player::GetTimerDuration() const
{
	return m_playerTimer->GetTotalDuration();
}

Miliseconds Player::GetTimerRemainingDuration()
{
	return m_playerTimer->GetRemainingDuration();
}

Miliseconds Player::GetTimerElapsedTime()
{
	return m_playerTimer->GetElapsedTime();
}

void Player::SetTimer(TimerPtr timer)
{
	m_playerTimer = timer;
}
