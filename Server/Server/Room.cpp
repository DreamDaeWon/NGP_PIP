#include "pch.h"
#include "Room.h"

// DW예정 : 일단 다 제작만 해둠 하나씩 바꿔나갈 예정
Room::Room() :
	CurrentMapSize{ 0 },
	mode{ RoomGameMode::ROOM_MODE_MAX }
{
	InitializeCriticalSection(&cs);
}

Room::~Room()
{
	DeleteCriticalSection(&cs);
}

void Room::AddPlayer(Session* player)
{
	Players.push_back(player);
}

void Room::RemovePlayer(Session* player)
{
	auto iter = std::find(Players.begin(), Players.end(), player);
	if(iter != Players.end())
	{
		Players.erase(iter);
	}
}

void Room::StartGame()
{

}

void Room::UpdateGame() // DW설명 : 플레이어 상태 갱신
{
	// 모든 플레이어의 상태 패킷 -> 이건 여기서만 갱신이 일어나기에 static으로 들고있기
	static AllPlayerState_Packet_S2C all_player_state{};

	// 플레이어 정보 (위치,상태,회전,id) 정보 받기
	size_t in_player_id{};
	while (incomingQueue.try_pop(in_player_id))
	{
		Session* player_session = Players[in_player_id];

		// DW예정 : session 클래스가 추가 된다면 플레이어 정보 받아올 예정
		Player player{};
		// player = player_session->player;

		// DW생각 : 플레이어가 3명밖에 없기 때문에 모든 플레이어의 상태를 한번에 갱신하는
		//			패킷을 만드는 것이 더 괜찮지 않을까?

		// 모든 플레이어의 상태 패킷을 하나로 만든다고 가정
		// 이게 MovePacket_s2c 이게 된다.

		all_player_state.x[player.id] = player.x;
		all_player_state.y[player.id] = player.y;
		all_player_state.state[player.id] = player.state;
		all_player_state.Rotate[player.id] = player.Rotate;
	}

	// 모든 플레이어의 상태를 담은 패킷을 브로드 캐스팅
	BroadcastPacket(all_player_state);
}

void Room::BroadcastPacket(AllPlayerState_Packet_S2C all_player)
{
	/*아마 이 방의 모든 상태를 담은 패킷을 전송할 예정*/
	
	// 스레드가 하나이므로 괜찮음
	// criticalsection 걸기
	EnterCriticalSection(&cs);

	for (Session* player : Players)
	{
		// all_player 패킷 전송 부분

		/* Session이 만들어지면 제작할 예정 */
		// player->SendPacket(all_player);
	}

	// criticalsection 해제
	LeaveCriticalSection(&cs);
}
