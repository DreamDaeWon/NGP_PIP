#pragma once
#include "Packet.h"

struct Player
{
	float x{};
	float y{};
	common::packet::Player_State state{ common::packet::Player_State::PLAYER_STATE_END };
	float Rotate{};
	size_t id{};
};
