#pragma once
#include "IClock.h"

#include <chrono>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>

using TimerExpiredCallback = std::function<void()>;

using TimerPtr = std::shared_ptr<class Timer>;

class Timer
{
public:
	static void SetClock(IClockPtr clock);

	Timer(Miliseconds duration, bool shouldStart = false, TimerExpiredCallback callback = nullptr);
	Timer() = delete;

	~Timer();

	enum class TimerState
	{
		Stopped,
		Running,
		Paused
	};

	void Start();
	void Stop();
	void Pause();

	void Reset();

	void SetExpiredCallback(TimerExpiredCallback callback);

	void SetDuration(Miliseconds duration);
	void AddDuration(Miliseconds duration);
	void SubtractDuration(Miliseconds duration);

	Miliseconds GetTotalDuration() const;
	Miliseconds GetRemainingDuration() const;
	Miliseconds GetElapsedTime() const;

	TimerState GetTimerState() const;

private://Timer instance
	void ExecuteCallback();

	TimerState m_state;

	Miliseconds m_duration;
	Miliseconds m_remainingTime;
	Timestamp m_lastUpdateTimestamp;

	TimerExpiredCallback m_callback;
	bool m_callbackExecuted;

	std::mutex m_mutex;

private://Timer class
	static void EnsureThreadRunning();
	static void EnsureThreadStopped();

	static void Run();

	static IClockPtr s_clock;

	static std::vector<Timer*> s_timers;

	static std::thread s_thread;
	static std::atomic<bool> s_running;
	static std::mutex s_mutex;

	static Miliseconds s_updateInterval;
	static std::condition_variable s_conditionalVariable;

	friend class TimerTests;
};
