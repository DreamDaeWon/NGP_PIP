#pragma once
#include "pch.h"
// DW예정 : 추후에 추가되면 추가할 예정인 헤더들
// #include "Session.h"
// #include "Time.h"

enum class RoomGameMode
{
	SinglePlayer,
	MutiPlayer,
	ROOM_MODE_MAX
};

// DW질문 : Player 구초제 관련 애들 어디에 정의해두어야 하지?
enum class Player_State
{
	IDLE,
	DIE,
	WAIT,
	SPACESHIP,
	ZIGZAG,
	PLAYER_STATE_END
};
struct Player
{
	float x{};
	float y{};
	Player_State state{ Player_State::PLAYER_STATE_END };
	float Rotate{};
	size_t id{};
};


struct AllPlayerState_Packet_S2C
{
	float x[4]{0.f,0.f,0.f,0.f};
	float y[4]{0.f,0.f,0.f,0.f};
	Player_State state[4]
	{ 
		Player_State::PLAYER_STATE_END,
		Player_State::PLAYER_STATE_END,
		Player_State::PLAYER_STATE_END,
		Player_State::PLAYER_STATE_END 
	};
	float Rotate[4]{0.f,0.f,0.f,0.f};
	enum PacketType; // = MovePacket_s2c
};



class Room
{
public:
	Room();
	~Room();

public:
	// DW예정 : 리턴 타입 및 매개변수 구현 때 하나 씩 수정할 예정
	void AddPlayer();
	void RemovePlayer();
	void StartGame();
	void UpdateGame();
	void BroadcastPacket(AllPlayerState_Packet_S2C all_player);


private:
	long long CurrentMapSize{};
	std::vector<class Session*> Players{};

	// DW생각 : 2vs2 모드가 없어졌기에 필요가 있나?
	// int Camera_ID;

	class Time* timer = nullptr;

	// DW질문 : 이거 어카지?
	Concurrency::concurrent_queue<size_t> incomingQueue;

	RoomGameMode mode{ RoomGameMode::ROOM_MODE_MAX };

	CRITICAL_SECTION cs;

};

