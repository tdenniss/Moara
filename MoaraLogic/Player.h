#pragma once
#include "IPlayer.h"

#include <cstdint>

class Player : public IPlayer
{
public:
	Player();
	Player(EPlayerType type, bool isComputer);
	~Player()
	{
		m_playerTimer->~Timer();
	}

	EPlayerType GetType() const override;
	void SetType(EPlayerType type) override;

	bool IsComputer() const override;
	void SetComputer(bool isComputer) override;

	void StartTimer();
	void StopTimer();
	void PauseTimer();
	void ResetTimer();
	void SetTimerCallback(TimerExpiredCallback callback);
	void SetTimerDuration(Miliseconds duration);
	void AddTimerDuration(Miliseconds duration);
	void SubtractTimerDuration(Miliseconds duration);
	Miliseconds GetTimerDuration() const;
	Miliseconds GetTimerRemainingDuration();
	Miliseconds GetTimerElapsedTime();

	void SetTimer(TimerPtr timer);
private:
	EPlayerType m_type;
	TimerPtr m_playerTimer;
	bool m_isComputer;
};