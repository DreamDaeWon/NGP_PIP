#pragma once
#include "Singleton.h"

class TimerManager : public Singleton<TimerManager>
{
public:
	TimerManager();
	~TimerManager();

public:
	void GetInstance();
	void DestroyInstance();

	void Reset();
	void Tick();

	float GetDeltaTime() const;
	float GetFPS() const;
	float GetTotalTime() const;
};

