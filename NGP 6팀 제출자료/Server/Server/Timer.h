#pragma once
#include "pch.h"

class Timer
{
public:
    Timer();
    ~Timer();

    // 틱 당 델타 타임(ms) 반환
    double GetDeltatime() const;

    // 스톱워치 시작
    void StartDeltatime();

    // 스톱워치 종료 및 시간(ms) 반환
    double EndDeltatime();

    // 매 틱마다 호출
    void Tick();

private:

    std::chrono::high_resolution_clock::time_point lastTickTime; // 이전 틱 시간
    double deltaTime; // 한 틱(프레임)간의 시간 (초 단위)

    std::chrono::high_resolution_clock::time_point startTime; // 시간 측정 시작 시간
};

