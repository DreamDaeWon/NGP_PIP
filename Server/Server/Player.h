#pragma once
#include "Packet.h"

struct Player
{
	long x{};
	long y{};
	common::packet::Player_State state{ common::packet::Player_State::PLAYER_STATE_END };
	float ridius{};
	float Rotate{};
	size_t id{};
};
