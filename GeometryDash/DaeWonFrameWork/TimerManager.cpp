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
	
}

float TimerManager::GetFPS() const
{
	
}

float TimerManager::SetTotalTime(float time)
{
	return _totalTime = time;
}

float TimerManager::GetTotalTime() const
{
	return _totalTime;
}