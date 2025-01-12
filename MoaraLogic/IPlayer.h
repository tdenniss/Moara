#pragma once
#include "EPieceType.h"

#include "Constants.h"
#include "Timer.h"

#include <memory>

using PlayerPtr = std::shared_ptr<class IPlayer>;
using EPlayerType = EPieceType;

class IPlayer
{
public:
	virtual EPlayerType GetType() const = 0;
	virtual void SetType(EPlayerType type) = 0;

	virtual bool IsComputer() const = 0;
	virtual void SetComputer(bool isComputer) = 0;

	virtual void StartTimer() = 0;
	virtual void StopTimer() = 0;
	virtual void PauseTimer() = 0;
	virtual void ResetTimer() = 0;
	virtual void SetTimerCallback(TimerExpiredCallback callback) = 0;
	virtual void SetTimerDuration(Miliseconds duration) = 0;
	virtual void AddTimerDuration(Miliseconds duration) = 0;
	virtual void SubtractTimerDuration(Miliseconds duration) = 0;
	virtual Miliseconds GetTimerDuration() const = 0;
	virtual Miliseconds GetTimerRemainingDuration() = 0;
	virtual Miliseconds GetTimerElapsedTime() = 0;

	virtual ~IPlayer() = default;
};