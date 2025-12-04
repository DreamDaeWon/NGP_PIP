#include "pch.h"
#include "Timer.h"


using namespace std::chrono;

Timer::Timer()
    : lastTickTime(high_resolution_clock::now())
    , deltaTime(0.0) // [추가된 멤버 변수] deltaTime을 0으로 초기화합니다.
{
 
}

Timer::~Timer()
{
}


double Timer::GetDeltatime() const
{
    return deltaTime;
}

void Timer::StartDeltatime()
{
    startTime = high_resolution_clock::now();
}


double Timer::EndDeltatime()
{
    auto endTime = high_resolution_clock::now();

    // 끝 시간과 시작 시간의 차이를 계산
    duration<double> elapsed = endTime - startTime;

    // '초' 단위 double 값으로 반환
    return elapsed.count();
}

void Timer::Tick()
{
    auto currentTime = high_resolution_clock::now();

    // 현재 시간과 마지막 틱 시간(lastTickTime)의 차이를 계산
    duration<double> elapsed = currentTime - lastTickTime;

    // 계산된 경과 시간(초 단위)을 deltaTime에 저장
    deltaTime = elapsed.count();

    // 다음 틱 계산을 위해 '마지막 틱 시간'을 현재 시간으로 업데이트
    lastTickTime = currentTime;
}