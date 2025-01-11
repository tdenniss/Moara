#pragma once
#include <chrono>
#include <memory>

using Seconds = std::chrono::seconds;
using Miliseconds = std::chrono::milliseconds;
using Timestamp = std::chrono::steady_clock::time_point;

using IClockPtr = std::shared_ptr<class IClock>;

class IClock
{
public:
	virtual Timestamp GetCurrentTime() = 0;
	virtual Miliseconds TimeBetween(Timestamp from, Timestamp to) = 0;
};