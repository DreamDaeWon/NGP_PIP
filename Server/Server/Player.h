#pragma once
#include "Packet.h"

struct Player
{
	float x{};
	float y{};
	Common::packet::Player_State state{ Common::packet::Player_State::PLAYER_STATE_END };
	float Rotate{};
	size_t id{};
};
