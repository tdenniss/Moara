#include "pch.h"

#include "Clock.h"

Timestamp Clock::GetCurrentTime()
{
	return std::chrono::steady_clock::now();
}

Miliseconds Clock::TimeBetween(Timestamp from, Timestamp to)
{
	return std::chrono::duration_cast<Miliseconds>(to - from);
}
