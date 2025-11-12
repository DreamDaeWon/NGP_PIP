#include "pch.h"
#include "Session.h"
#include "Room.h"
#include "Server.h"
#include "Timer.h"

// DW예정 : 일단 다 제작만 해둠 하나씩 바꿔나갈 예정
Room::Room() :
	CurrentMapSize{ 0 },
	mode{ RoomGameMode::ROOM_MODE_MAX },
	timer{ new Timer() }
{
	RegisterHandler(common::packet::PacketType::MovePacket_c2s, &Room::MovePacket_c2s);
}

Room::~Room()
{
	
}

void Room::AddPlayer(Session* player)
{
	_playerMutex.lock();
	Players.push_back(player);
	_playerMutex.unlock();
}

void Room::RemovePlayer(Session* player)
{
	_playerMutex.lock();
	auto iter = std::find(Players.begin(), Players.end(), player);
	if(iter != Players.end())
	{
		Players.erase(iter);
	}
	_playerMutex.unlock();
}

void Room::StartGame()
{
	_isGameRunning = true;
}

void Room::StopGame()
{
	_isGameRunning = false;
}

void Room::UpdateGame() // DW설명 : 플레이어 상태 갱신
{
	StartGame(); // 게임 루프 시작
	const float FPS = 60.0f;
	const float FRAME_TIME = 1.0f / FPS;

	while (_isGameRunning)
	{
		timer->Tick();

		// 패킷 입력 처리
		ProcessInputs();
		// 모든 플레이어 상태 브로드캐스트
		BroadcastState();

		// 3. 루프 주기 맞추기
		double elapsedTime = timer->EndDeltatime();
		if (elapsedTime < FRAME_TIME)
		{
			std::this_thread::sleep_for(std::chrono::duration<double>(FRAME_TIME - elapsedTime));
		}
	}
}

void Room::BroadcastPacket(common::packet::PacketHeader* packet)
{
	std::lock_guard<std::mutex> lock(_playerMutex);
	for (Session* player : Players)
	{
		// KJ: 이 메모리는 각 세션의 SendPacket 함수에서 delete
		char* buffer = new char[packet->size];
		memcpy(buffer, packet, packet->size);
		player->EnqueuePacket(reinterpret_cast<common::packet::PacketHeader*>(buffer));
	}
}

void Room::EnqueuePacket(common::packet::PacketHeader* packet)
{
	_queueMutex.lock();
	incomingQueue.push(packet);
	_queueMutex.unlock();
}
// KJ: 원래 UpdateLoop의 매 루프마다 해주던 패킷 처리 작업을 함수로 분리
void Room::ProcessInputs()
{
	// 이번 프레임에 처리할 패킷들을 임시 큐로 옮김
	std::queue<common::packet::PacketHeader*> packetsToProcess;
	{
		std::lock_guard<std::mutex> lock(_queueMutex);
		packetsToProcess.swap(incomingQueue);
	} // 여기서 _queue_mutex 락이 해제됩니다.


	// 임시 큐의 패킷들을 모두 처리
	while (!packetsToProcess.empty())
	{
		common::packet::PacketHeader* packet = packetsToProcess.front();
		packetsToProcess.pop();

		// C2S_MovePacket만 처리한다고 가정
		
		HandlePacket(nullptr, reinterpret_cast<char*>(packet));

		delete packet;
	}
}

//KJ: 원래 UpdateLoop의 매 루프마다 해주던 패킷 만들어주는 작업을 함수로 분리
void Room::BroadcastState()
{
	common::packet::S2C_AllPlayerMovePacket allPlayerStatePacket{};
	allPlayerStatePacket.size = sizeof(common::packet::S2C_AllPlayerMovePacket);
	allPlayerStatePacket.type = common::packet::PacketType::AllPlayerMovePacket_s2c;

	_playerMutex.lock();
	for (const auto& session : Players)
	{
		const Player& player = session->getPlayer();
		size_t id = player.id;
		if (id < MAX_PLAYERS)
		{
			allPlayerStatePacket.x[id] = player.x;
			allPlayerStatePacket.y[id] = player.y;
			allPlayerStatePacket.state[id] = player.state;
			allPlayerStatePacket.Rotate[id] = player.Rotate;
		}
	}
	_playerMutex.unlock();

	// BroadcastPacket 함수를 통해 모든 플레이어에게 전송
	BroadcastPacket(reinterpret_cast<common::packet::PacketHeader*>(&allPlayerStatePacket));
}

void Room::RegisterHandler(common::packet::PacketType type, PacketHandlerFunc func)
{
	Handlers[type] = func;
}

// 추가된 오버로드: Room 멤버 함수 포인터를 받아 this로 바인딩해서 저장
void Room::RegisterHandler(common::packet::PacketType type, void (Room::* func)(Session*, char*))
{
	using namespace std::placeholders;
	Handlers[type] = std::bind(func, this, _1, _2);
}

void Room::HandlePacket(Session* session, char* packet)
{
	common::packet::PacketHeader* header = reinterpret_cast<common::packet::PacketHeader*>(packet);
	auto it = Handlers.find(header->type);
	if (it != Handlers.end())
	{
		PacketHandlerFunc func = it->second;
		func(session, packet);
	}
	else
	{
		// 알 수 없는 패킷 타입 처리
	}
}

void Room::MovePacket_c2s(Session* player, char* packet)
{
	common::packet::C2S_MovePacket* movePacket = reinterpret_cast<common::packet::C2S_MovePacket*>(packet);
	// ID 유효성 검사
	{
		std::lock_guard<std::mutex> lock(_playerMutex); // Players 벡터 접근을 위한 락
		// movePacket->id가 Players 벡터의 유효한 인덱스인지 확인
		if (movePacket->id < Players.size())
		{
			// 세션 포인터를 통해 해당 플레이어의 상태를 직접 업데이트
			Player& player = Players[movePacket->id]->getPlayer();
			player.x = movePacket->x;
			player.y = movePacket->y;
			player.state = movePacket->player_state;
			player.Rotate = movePacket->rotate;
		}
	} // _players_mutex 락 해제
}

void Room::LoginRequestPacket_c2s(Session* player, char* packet)
{
	// 1150 : 고민중
}
