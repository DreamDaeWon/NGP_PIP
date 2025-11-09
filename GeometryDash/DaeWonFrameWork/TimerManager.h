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

	// 서버에서 시간 측정하면 클라에선 아래 두개만 필요하지않나??
	float SetTotalTime(float time);
	float GetTotalTime() const;
private:
	float _totalTime;
};

