#pragma once
#include "Packet.h"

struct Player
{
	long x{};
	long y{};
	float vx{};
	float vy{};
	common::packet::Player_State state{ common::packet::Player_State::PLAYER_STATE_END };
	float ridius{};
	float Rotate{};
	size_t id{};
	bool isActive = false; // 플레이어 활성 상태 플래그
};