#include "TimerManager.h"

TimerManager::TimerManager()
	:_totalTime(0.0f)
{
}

TimerManager::~TimerManager()
{
}

void TimerManager::init()
{
	_totalTime = 0.f;
	_lastTimePoint = std::chrono::steady_clock::now();
	_currentTimePoint = _lastTimePoint;
}


void TimerManager::Reset()
{
	_totalTime = 0.f;
	_lastTimePoint = std::chrono::steady_clock::now();
}

void TimerManager::Tick()
{
	_currentTimePoint = std::chrono::steady_clock::now();
	std::chrono::duration<float> elapsed = _currentTimePoint - _lastTimePoint;
	_deltaTime = elapsed.count();
	_lastTimePoint = _currentTimePoint;
	_totalTime += _deltaTime;
}

float TimerManager::GetDeltaTime() const
{
	return _deltaTime;
}

float TimerManager::GetFPS() const
{
	return 1.f / _deltaTime;
}

float TimerManager::SetTotalTime(float time)
{
	return _totalTime = time;
}

float TimerManager::GetTotalTime() const
{
	return _totalTime;
}