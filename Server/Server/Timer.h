#pragma once
#include "pch.h"

class Timer
{
public:
	Timer();
	~Timer();
public:
	void GetDeltatime();
	void StartDeltatime();
	void EndDeltatime();
	void Tick();

private:
	double deltaTime{};
};

