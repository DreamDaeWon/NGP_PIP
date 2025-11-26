#pragma once
#include <cstdint> 

// CJ : 공용 구조체, 패킷 정의하는 곳
constexpr uint32_t MAX_PLAYERS = 3;
constexpr char SERVER_IP[] = "127.0.0.1";
constexpr short SERVER_PORT = 9000;

namespace common::packet 
{
	enum class PacketType : uint16_t {
		ErrorPacket = 0,
		
		// s2c는 짝수, c2s는 홀수

		// Move관련 패킷은 10번 대
		MovePacket_s2c = 10, 
		AllPlayerMovePacket_s2c = 11,
		MovePacket_c2s = 12, 


		// Login/out관련 패킷은 20번 대
		LoginAcceptPacket_s2c = 20,
		LoginRequestPacket_c2s = 21,
		LogoutPacket_c2s = 23,

		// Map관련 패킷은 30번 대

		// Room관련 패킷은 40번 대
		RoomWaitPacket_s2c = 40,
		RoomStartPacket_s2c = 42
	};
	enum class Player_State : uint8_t
	{
		IDLE				= 0, // normal
		SPACESHIP			= 1, // AIRPLANE
		ZIGZAG				= 2, // 
		FINISH				= 3,
		DIE					= 4,
		PLAYER_STATE_END	= 5
	};
	// 1바이트 크기로 정렬
#pragma pack(push, 1)

	struct PacketHeader
	{
		long long size; // 해당 헤더를 포함한 패킷의 전체 크기
		PacketType type; // 패킷 타입
	};
	
	// -------------------------------------------Client to Server-------------------------------------------

	struct C2S_MovePacket : public PacketHeader {
		int id;
		long x, y;
		float rotate;
		float ridius; // 반지름 보내기
		int cameraID;
		Player_State player_state;
	};
	struct C2S_LogoutPacket : public PacketHeader {
		int	id;
	};
	struct C2S_LoginRequestPacket : public PacketHeader {
		int	id;
	};
	// -------------------------------------------Server to Client-------------------------------------------
	
	struct S2C_MovePacket : public PacketHeader {
		int id;
		float x, y;
		float rotate;
		Player_State player_state; 
		float minMapPercentage; // 이거는 minMap을 통해 해당 플레이어의 위치를 짐작할 수 있도록 제작하기 + 백분율로!
	};

	struct S2C_AllPlayerMovePacket : public PacketHeader
	{
		float x[3]{ 0.f,0.f,0.f };
		float y[3]{ 0.f,0.f,0.f };
		float ridius[3]{ 0.f,0.f,0.f };
		Player_State state[3]
		{
			Player_State::PLAYER_STATE_END,
			Player_State::PLAYER_STATE_END,
			Player_State::PLAYER_STATE_END,
		};
		float Rotate[3]{ 0.f,0.f,0.f };
	};

	struct S2C_RoomWaitPacket : public PacketHeader {
		int playerCount;
		int playerIDs[MAX_PLAYERS]; // 빈 슬롯은 -1
	};

	struct S2C_RoomStartPacket : public PacketHeader {
		int playerCount;
		int playerIDs[MAX_PLAYERS];
	};

	struct S2C_LoginAcceptPacket : public PacketHeader {
		int	playerID;
	};
#pragma pack (pop)
}