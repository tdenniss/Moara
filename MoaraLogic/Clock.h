#pragma once
#include "IClock.h"

class Clock : public IClock
{
public:
	Timestamp GetCurrentTime() override;
	Miliseconds TimeBetween(Timestamp from, Timestamp to) override;
};