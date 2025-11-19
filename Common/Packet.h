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
		/*MapNamesPacket_s2c = 30, 
		MapLoadRequestPacket_c2s = 31,
		MapLoadPacket_s2c = 32, */

		MapRoomLeavePacket_c2s = 33,
		MapRoomEndPacket_s2c = 34,

		// Room관련 패킷은 40번 대
		RoomEnterAcceptPacket_s2c = 40,
		RoomEnterPacket_c2s = 41,  
		RoomStartPacket_s2c = 42

		//Map업로드는 필요 없으니 주석
		//MapUploadPacket_c2s = 1, 
		//MapUploadDonePacket_s2c = 2, 
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

	// CJ : 이 패킷은 이제 우리 유즈맵 만들어서 업로드 하는 기능은 필요 없으니 제외해도될듯
	/*struct C2S_MapNamesPacket : public PacketHeader {
		std::array<char[30], 10> mapNames;
		std::vector<char> mapData; 
	};*/

	struct C2S_LoginAcceptPacket : public PacketHeader {
		int playerID;
	};

	struct C2S_RoomEnterAcceptPacket : public PacketHeader {
		//KJ: 패킷 타입으로 알아볼수 있음
	};

	struct C2S_RoomStartPacket : public PacketHeader {
		//KJ: 패킷 타입으로 알아볼수 있음
	};

	// CJ : 이것도 C2S_MapNamesPacket와 마찬가지
	//struct C2S_UploadDonePacket : public PacketHeader {
	//	//KJ: 패킷 타입으로 알아볼수 있음
	//};

	struct C2S_RoomEndPacket : public PacketHeader {
		int roomID;
		int winUserID;
		float ElapsedTime; // CJ 궁금 : 서버에서 시간 측정해서 뿌려주기로 했는데 구조 설명해줄 수 있는 사람
	};

	// -------------------------------------------Server to Client-------------------------------------------
	
	struct S2C_MovePacket : public PacketHeader {
		int id;
		float x, y;
		float rotate;
		Player_State player_state; // <- CJ : Player_State 제작 후 주석 해제
		float minMapPercentage; // 이거는 minMap을 통해 해당 플레이어의 위치를 짐작할 수 있도록 제작하기 + 백분율로!
	};

	struct S2C_AllPlayerMovePacket : public PacketHeader
	{
		float x[3]{ 0.f,0.f,0.f };
		float y[3]{ 0.f,0.f,0.f };

		Player_State state[3]
		{
			Player_State::PLAYER_STATE_END,
			Player_State::PLAYER_STATE_END,
			Player_State::PLAYER_STATE_END,
		};
		float Rotate[3]{ 0.f,0.f,0.f };
	};

	struct S2C_LoginRequestPacket : public PacketHeader {
		//KJ: 패킷 타입으로 알아볼수 있음
	};

	struct S2C_LogoutPacket : public PacketHeader {
		//KJ: 패킷 타입으로 알아볼수 있음
	};

	// CJ : C2S_MapNamesPacket 처럼 얘도 필요 없어질 듯
	/*struct S2C_MapLoadRequestPacket : public PacketHeader {
		char mapName[30];
	};*/

	struct S2C_RoomEnterPacket : public PacketHeader {
		int roomID;
	};

	// CJ : C2S_MapNamesPacket 처럼 얘도 필요 없어질 듯
	/*struct S2C_MapUploadPacket : public PacketHeader {
		char mapName[30];
		std::vector<char> mapData;
	};*/

	struct S2C_RoomLeavePacket : public PacketHeader {
		int romID;
	};
#pragma pack (pop)
}