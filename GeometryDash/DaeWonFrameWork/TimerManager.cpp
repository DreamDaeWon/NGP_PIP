#include "TimerManager.h"

TimerManager::TimerManager()
	:_totalTime(0.0f)
{
}

TimerManager::~TimerManager()
{
}

void TimerManager::GetInstance()
{
}

void TimerManager::DestroyInstance()
{
}

void TimerManager::Reset()
{
}

void TimerManager::Tick()
{
}

float TimerManager::GetDeltaTime() const
{
	return 0.f;
}

float TimerManager::GetFPS() const
{
	return 0.f;
}

float TimerManager::SetTotalTime(float time)
{
	return _totalTime = time;
}

float TimerManager::GetTotalTime() const
{
	return _totalTime;
}