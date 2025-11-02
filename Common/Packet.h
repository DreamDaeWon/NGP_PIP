#pragma once
#include <cstdint> 
#include <array> // C2S_MapNamesPacket 구조체 땜에 선언해둔거니 나중에 삭제할 때 같이 삭제

// CJ : 공용 구조체, 패킷 정의하는 곳

namespace Common::packet 
{
	enum class PacketType : uint16_t {
		ErrorPacket = 0,
		
		// s2c는 짝수, c2s는 홀수

		// Move관련 패킷은 10번 대
		MovePacket_s2c = 10, 
		MovePacket_c2s = 11, 

		// Login/out관련 패킷은 20번 대
		LoginAcceptPacket_s2c = 20,
		LoginRequestPacket_c2s = 21,
		LogoutPacket_c2s = 23,

		// Map관련 패킷은 30번 대
		MapNamesPacket_s2c = 30, 
		MapLoadRequestPacket_c2s = 31,
		MapLoadPacket_s2c = 32, 

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

	// 1바이트 크기로 정렬
	#pragma pack(push, 1)

	struct PacketHeader
	{
		uint16_t size; // 해당 헤더를 포함한 패킷의 전체 크기
		PacketType type; // 패킷 타입
	};

	// -------------------------------------------Client to Server-------------------------------------------

	struct C2S_MovePacket {
		long long size;
		PacketType type;
		float x, y;
		int id;
		float rotate;
		int cameraID;
		//Player_State player_state; <- CJ : Player_State 제작 후 주석 해제
		float minMapPercentage; // 이거는 minMap을 통해 해당 플레이어의 위치를 짐작할 수 있도록 제작하기 + 백분율로!
	};

	// CJ : 이 패킷은 이제 우리 유즈맵 만들어서 업로드 하는 기능은 필요 없으니 제외해도될듯
	struct C2S_MapNamesPacket {
		long long size;
		PacketType type;
		std::array<char[30], 10> mapNames;
		std::vector<char> mapData; 
	};

	struct C2S_LoginAccpetPacket {
		long long size;
		PacketType type;
		int playerID;
	};

	struct C2S_RoomEnterAcceptPacket {
		long long size;
		PacketType type;
	};

	struct C2S_RoomStartPacket {
		long long size;
		PacketType type;
	};

	// CJ : 이것도 C2S_MapNamesPacket와 마찬가지
	struct C2S_UploadDonePacket {
		long long size;
		PacketType type;
	};

	struct C2S_RoomEndPacket {
		long long size;
		PacketType type;
		int roomID;
		int winUserID;
		float ElapsedTime; // CJ 궁금 : 서버에서 시간 측정해서 뿌려주기로 했는데 구조 설명해줄 수 있는 사람
	};

	// -------------------------------------------Server to Client-------------------------------------------

	struct S2C_MovePacket {
		long long size;
		PacketType type;
		float x, y;
		int id;
		float rotate;
		//Player_State player_state; <- CJ : Player_State 제작 후 주석 해제
		// float x, y; CJ 궁금 : 여기 float x, y를 두개 선언해주는데 적 위치 때문인가? 일단 주석 처리해둠
	};

	struct S2C_LoginRequestPacket {
		long long size;
		PacketType type;
	};

	struct S2C_LogoutPacket {
		long long size;
		PacketType type;
	};

	// CJ : C2S_MapNamesPacket 처럼 얘도 필요 없어질 듯
	struct S2C_MapLoadRequestPacket {
		long long size;
		PacketType type;
		char mapName[30];
	};

	struct S2C_RoomEnterPacket {
		long long size;
		PacketType type;
		int roomID;
	};

	// CJ : C2S_MapNamesPacket 처럼 얘도 필요 없어질 듯
	struct S2C_MapUploadPacket {
		long long size;
		PacketType type;
		char mapName[30];
		std::vector<char> mapData;
	};

	struct S2C_RoomLeavePacket {
		long long size;
		PacketType type;
		int romID;
	};
}