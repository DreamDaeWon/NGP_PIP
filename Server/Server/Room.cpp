#include "pch.h"
#include "Session.h"
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
	static common::packet::S2C_AllPlayerMovePacket all_player_state{};

	// 플레이어 정보 (위치,상태,회전,id) 정보 받기
	common::packet::PacketHeader* in_move_packet{};
	while (true)
	{
		EnterCriticalSection(&cs);
		in_move_packet = incomingQueue.front();
		incomingQueue.pop();
		LeaveCriticalSection(&cs);

		// DW생각 : 플레이어가 3명 이하라면 이 UpdateGame루프는 무시
		//			왜냐? -> 이것은 게임이 시작된 후에 달리는 루프이기 때문이다. 대기방이 아님.
		if (Players.size() < 3)
		{
			continue;
		}

		common::packet::C2S_MovePacket* move_packet = reinterpret_cast<common::packet::C2S_MovePacket*>(in_move_packet);

		Session* player_session = Players[move_packet->id];

		// DW예정 : session 클래스가 추가 된다면 플레이어 정보 받아올 예정
		Player player{};
		player = player_session->getPlayer();

		// DW생각 : 플레이어가 3명밖에 없기 때문에 모든 플레이어의 상태를 한번에 갱신하는
		//			패킷을 만드는 것이 더 괜찮지 않을까?

		// 모든 플레이어의 상태 패킷을 하나로 만든다고 가정 -> 이게 S2C_AllPlayerMovePacket 이게 된다.

		player.x = move_packet->x;
		player.y = move_packet->y;
		player.state = move_packet->player_state;
		player.Rotate = move_packet->rotate;

		all_player_state.x[player.id] = player.x;
		all_player_state.y[player.id] = player.y;
		all_player_state.state[player.id] = player.state;
		all_player_state.Rotate[player.id] = player.Rotate;


		// DW생각 : 메모리 누수 방지 왜냐? new 로 패킷을 보내기 때문
		delete in_move_packet;
	}

	// 모든 플레이어의 상태를 담은 패킷을 브로드 캐스팅
	BroadcastPacket(all_player_state);
}

void Room::BroadcastPacket(common::packet::S2C_AllPlayerMovePacket all_player)
{
	common::packet::PacketHeader* out_move_packet = new common::packet::S2C_AllPlayerMovePacket();
	memcpy(out_move_packet, &all_player, sizeof(common::packet::S2C_AllPlayerMovePacket));

	/*이 방의 모든 상태를 담은 패킷을 전송*/
	for (Session* player : Players)
	{
		// all_player 패킷 전송 부분
		player->EnqueuePacket(reinterpret_cast<common::packet::PacketHeader*>(&all_player));
	}
}

void Room::EnqueuePacket(common::packet::PacketHeader* packet)
{
	EnterCriticalSection(&cs);
	incomingQueue.push(packet);
	LeaveCriticalSection(&cs);
}
