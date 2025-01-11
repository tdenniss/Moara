#include "pch.h"

#include "Timer.h"
#include "Clock.h"

IClockPtr Timer::s_clock{ std::make_shared<Clock>() };
std::vector<Timer*> Timer::s_timers;
std::thread Timer::s_thread;
std::atomic<bool> Timer::s_running{ false };
std::mutex Timer::s_mutex;
Miliseconds Timer::s_updateInterval{ 5000 };
std::condition_variable Timer::s_conditionalVariable;

void Timer::SetClock(IClockPtr clock)
{
	s_clock = clock;
}

Timer::Timer(Miliseconds duration, bool shouldStart /*= false*/, TimerExpiredCallback callback /*= nullptr*/)
	: m_duration(duration)
	, m_callback(callback)
	, m_callbackExecuted(false)
	, m_remainingTime(duration)
	, m_lastUpdateTimestamp(s_clock->GetCurrentTime())
	, m_state(shouldStart ? TimerState::Running : TimerState::Stopped)
{
	{
		std::unique_lock<std::mutex> lock(s_mutex);
		s_timers.push_back(this);
	}

	EnsureThreadRunning();
}

Timer::~Timer()
{
	{
		std::unique_lock<std::mutex> lock(s_mutex);

		s_timers.erase(std::remove(s_timers.begin(), s_timers.end(), this), s_timers.end());
	}

	EnsureThreadStopped();
}

void Timer::Start()
{
	std::lock_guard<std::mutex> lock(m_mutex);

	if (m_state == Timer::TimerState::Running)
		return;

	if (m_state == Timer::TimerState::Stopped)
	{
		m_remainingTime = m_duration;
		m_callbackExecuted = false;
	}

	// Stopped or Paused
	m_lastUpdateTimestamp = s_clock->GetCurrentTime();
	m_state = TimerState::Running;

	s_conditionalVariable.notify_one();
}

void Timer::Stop()
{
	std::lock_guard<std::mutex> lock(m_mutex);

	m_state = TimerState::Stopped;
}

void Timer::Pause()
{
	std::lock_guard<std::mutex> lock(m_mutex);

	if (m_state == TimerState::Running)
	{
		m_state = TimerState::Paused;

		m_remainingTime -= s_clock->TimeBetween(m_lastUpdateTimestamp, s_clock->GetCurrentTime());
		if (m_remainingTime < Miliseconds(0))
			m_remainingTime = Miliseconds(0);
	}
}

void Timer::Reset()
{
	std::lock_guard<std::mutex> lock(m_mutex);

	m_lastUpdateTimestamp = s_clock->GetCurrentTime();
	m_state = TimerState::Running;
	m_remainingTime = m_duration;
	m_callbackExecuted = false;

	s_conditionalVariable.notify_one();
}

void Timer::SetExpiredCallback(TimerExpiredCallback callback)
{
	std::lock_guard<std::mutex> lock(m_mutex);

	m_callback = callback;
}

void Timer::SetDuration(Miliseconds duration)
{
	std::lock_guard<std::mutex> lock(m_mutex);

	m_remainingTime = m_duration = duration;

	s_conditionalVariable.notify_one();
}

void Timer::AddDuration(Miliseconds duration)
{
	std::lock_guard<std::mutex> lock(m_mutex);

	m_duration += duration;
	m_remainingTime += duration;

	s_conditionalVariable.notify_one();
}

void Timer::SubtractDuration(Miliseconds duration)
{
	std::lock_guard<std::mutex> lock(m_mutex);

	m_duration -= duration;
	m_remainingTime -= duration;

	s_conditionalVariable.notify_one();
}

Miliseconds Timer::GetTotalDuration() const
{
	return m_duration;
}

Miliseconds Timer::GetRemainingDuration() const
{
	if (m_state == TimerState::Running)
		return std::max<Miliseconds>(m_remainingTime - s_clock->TimeBetween(m_lastUpdateTimestamp, s_clock->GetCurrentTime()), Miliseconds(0));

	return m_remainingTime;
}

Miliseconds Timer::GetElapsedTime() const
{
	if (m_state == TimerState::Running)
		return std::max<Miliseconds>(m_duration - m_remainingTime + s_clock->TimeBetween(m_lastUpdateTimestamp, s_clock->GetCurrentTime()), Miliseconds(0));

	return m_duration - m_remainingTime;
}

Timer::TimerState Timer::GetTimerState() const
{
	return m_state;
}

void Timer::ExecuteCallback()
{
	if (m_callbackExecuted)
		return;

	m_callbackExecuted = true;

	if (m_callback)
		m_callback();
}

void Timer::EnsureThreadRunning()
{
	std::lock_guard<std::mutex> lock(s_mutex);

	if (!s_thread.joinable())//if the thread already exists do nothing
	{
		s_running = true;
		s_thread = std::thread(&Timer::Run);
	}
}

void Timer::EnsureThreadStopped()
{
	{
		std::lock_guard<std::mutex> lock(s_mutex);

		if (!s_timers.empty())//if there are timers running do nothing
			return;

		s_running = false;

		s_conditionalVariable.notify_all();
	}
	if (s_thread.joinable())
		s_thread.join();
}

void Timer::Run()
{
	do
	{
		std::unique_lock<std::mutex> lock(s_mutex);

		Miliseconds minimumDuration = s_updateInterval;

		std::vector<TimerExpiredCallback> callbacks;
		for (const auto& timer : s_timers)
		{
			if (timer->m_state != TimerState::Running)
				continue;

			Miliseconds timerRemainingDuration = timer->GetRemainingDuration();
			timer->m_lastUpdateTimestamp = s_clock->GetCurrentTime();
			timer->m_remainingTime = timerRemainingDuration;

			if (timerRemainingDuration < minimumDuration)
				minimumDuration = timerRemainingDuration;

			if (timerRemainingDuration <= Miliseconds(0))
			{
				callbacks.push_back([timer]() { timer->ExecuteCallback(); });
				timer->Stop();
			}
		}

		Timestamp timeBeforeCallbacks = s_clock->GetCurrentTime();
		lock.unlock();
		for (const auto& callback : callbacks)
			callback();
		lock.lock();
		Timestamp timeAfterCallbacks = s_clock->GetCurrentTime();

		minimumDuration -= s_clock->TimeBetween(timeBeforeCallbacks, timeAfterCallbacks);

		if (!s_running)
			return;

		s_conditionalVariable.wait_for(lock, minimumDuration);
	} while (s_running);
}
