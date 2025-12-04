#pragma once
#include <chrono>

#include "Singleton.h"

class TimerManager : public Singleton<TimerManager>
{
public:
	TimerManager();
	virtual ~TimerManager();

public:
	void init();
	void Reset();
	void Tick();

	float GetDeltaTime() const;
	float GetFPS() const;

	// CJ질문 : 서버에서 시간 측정하면 클라에선 아래 두개만 필요하지않나??
	float SetTotalTime(float time);
	float GetTotalTime() const;
private:
	float _totalTime; // 서버에서 주는 방 돌아가던 시간
	std::chrono::steady_clock::time_point _lastTimePoint;
	std::chrono::steady_clock::time_point _currentTimePoint;
	float _deltaTime;
};

