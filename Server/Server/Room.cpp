#include "pch.h"
#include "Session.h"
#include "Room.h"

#include <utility>
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
	delete timer;

	// incomingQueue의 unique_ptr들이 자동으로 메모리를 해제하므로,
	// 별도의 반복문으로 큐를 비울 필요가 없습니다.
}

void Room::AddPlayer()
{
	std::lock_guard<std::mutex> lock(_playerMutex);
	Players.push_back(Player{});
}

void Room::RemovePlayer(int id)
{
	std::lock_guard<std::mutex> lock(_playerMutex);
	std::erase_if(Players, [id](const Player& player)
		{
			return std::cmp_equal(player.id, id);
		});
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
		timer->StartDeltatime();

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
	// Create the packet data once
	auto packetData = std::make_shared<std::vector<char>>(packet->size);
	memcpy(packetData->data(), packet, packet->size);

	// Lock and broadcast to all sessions
	std::lock_guard<std::mutex> lock(Server::Instance()->_clients._sessionLock);
	auto& sessions = Server::Instance()->_clients._sessions;
	for (auto& session : sessions)
	{
		// Enqueue the same shared_ptr to all sessions.
		// This avoids copying the data for each session.
		if (session.isConnected())
		{
			session.EnqueuePacket(packetData);
		}
	}
}

void Room::EnqueuePacket(std::unique_ptr<common::packet::PacketHeader> packet)
{
	std::lock_guard lock(_queueMutex);
	incomingQueue.push(std::move(packet));
}
// KJ: 원래 UpdateLoop의 매 루프마다 해주던 패킷 처리 작업을 함수로 분리
void Room::ProcessInputs()
{
	// 이번 프레임에 처리할 패킷들을 임시 큐로 옮김
	std::queue<std::unique_ptr<common::packet::PacketHeader>> packetsToProcess;
	{
		std::lock_guard lock(_queueMutex);
		packetsToProcess.swap(incomingQueue);
	} // 여기서 _queue_mutex 락이 해제됩니다.


	// 임시 큐의 패킷들을 모두 처리
	while (!packetsToProcess.empty())
	{
		std::unique_ptr<common::packet::PacketHeader> packet = std::move(packetsToProcess.front());
		packetsToProcess.pop();

		// C2S_MovePacket만 처리한다고 가정
		
		HandlePacket(nullptr, reinterpret_cast<char*>(packet.get()));

		// unique_ptr이 범위를 벗어나면 자동으로 메모리가 해제됩니다.
	}
}

//KJ: 원래 UpdateLoop의 매 루프마다 해주던 패킷 만들어주는 작업을 함수로 분리
void Room::BroadcastState()
{
	common::packet::S2C_AllPlayerMovePacket allPlayerStatePacket{};
	allPlayerStatePacket.size = sizeof(common::packet::S2C_AllPlayerMovePacket);
	allPlayerStatePacket.type = common::packet::PacketType::AllPlayerMovePacket_s2c;

	
	{
		std::lock_guard lock(_playerMutex);
		for (const auto& player : Players)
		{
			size_t id = player.id;
			if (id < MAX_PLAYERS)
			{
				allPlayerStatePacket.x[id] = player.x;
				allPlayerStatePacket.y[id] = player.y;
				allPlayerStatePacket.state[id] = player.state;
				allPlayerStatePacket.Rotate[id] = player.Rotate;
			}
		}
	}

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
	Handlers[type] = std::bind(func,this, _1, _2);
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

void Room::MovePacket_c2s(Session* session, char* packet)
{
	common::packet::C2S_MovePacket* movePacket = reinterpret_cast<common::packet::C2S_MovePacket*>(packet);
	
	if (movePacket->id < Players.size())
	{
		// 세션 포인터를 통해 해당 플레이어의 상태를 직접 업데이트
		Player& player = Players[movePacket->id];
		player.x = movePacket->x;
		player.y = movePacket->y;
		player.state = movePacket->player_state;
		player.Rotate = movePacket->rotate;
	}
	
}