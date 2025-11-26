#include "pch.h"
#include "Session.h"
#include "Room.h"
#include "Server.h"
#include "Timer.h"

// Room 클래스 생성자 및 소멸자
Room::Room() : timer{ new Timer() }
{
    // Players 배열 초기화 (기본적으로 비활성 상태)
    for (int i = 0; i < MAX_PLAYERS; ++i)
    {
        Players[i].id = i;
        Players[i].isActive = false; // Player 구조체에 isActive 필드가 있다고 가정
        Players[i].state = common::packet::Player_State::PLAYER_STATE_END;
        // 다른 필드들도 기본값으로 초기화
    }
	RegisterHandler(common::packet::PacketType::MovePacket_c2s, &Room::MovePacket_c2s);
}

Room::~Room()
{
	delete timer;
	// incomingQueue의 unique_ptr들이 자동으로 메모리를 해제하므로,
	// 별도의 반복문으로 큐를 비울 필요가 없습니다.
}

// 플레이어 추가 (ID 기반)
void Room::AddPlayer(int id)
{
	std::lock_guard<std::mutex> lock(_playerMutex);
    if (id >= 0 && id < MAX_PLAYERS)
    {
        Players[id] = Player(); // Player 객체 기본 생성자로 초기화
        Players[id].id = id;
        Players[id].isActive = true; // 활성 상태로 설정
        Players[id].state = common::packet::Player_State::IDLE; // 초기 상태 설정
        // 다른 필드들도 적절히 초기화 (예: x, y 좌표)
        printf("Room에 플레이어 ID %d 추가 및 활성화.\n", id);
    }
    else
    {
        printf("[경고] Room::AddPlayer - 유효하지 않은 ID: %d\n", id);
    }
}

// 플레이어 제거 (ID 기반)
void Room::RemovePlayer(int id)
{
	std::lock_guard<std::mutex> lock(_playerMutex);
    if (id >= 0 && id < MAX_PLAYERS)
    {
        if (Players[id].isActive) // 활성화된 플레이어인 경우만 처리
        {
            printf("Room에서 플레이어 ID %d 제거 및 비활성화.\n", id);
            Players[id].isActive = false; // 비활성 상태로 설정
            Players[id].state = common::packet::Player_State::PLAYER_STATE_END; // 상태 초기화
            // 나머지 플레이어 데이터(x, y 등)도 초기화할 수 있음

            // 다른 클라이언트들에게 퇴장 패킷 전송
            common::packet::S2C_DespawnOtherPlayerPacket despawnPacket;
            despawnPacket.type = common::packet::PacketType::DespawnOtherPlayerPacket_s2c;
            despawnPacket.size = sizeof(common::packet::S2C_DespawnOtherPlayerPacket);
            despawnPacket.OtherplayerID = id;
            BroadcastPacket(reinterpret_cast<common::packet::PacketHeader*>(&despawnPacket));
        }
    }
    else
    {
        printf("[경고] Room::RemovePlayer - 유효하지 않은 ID: %d\n", id);
    }
}

void Room::StartGame()
{
	_isGameRunning = true;
}

void Room::StopGame()
{
	_isGameRunning = false;
}

// 플레이어 상태 갱신
void Room::UpdateGame() 
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

// 매 루프마다 해주던 패킷 처리 작업을 함수로 분리
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

// 매 루프마다 해주던 패킷 만들어주는 작업을 함수로 분리
void Room::BroadcastState()
{
	common::packet::S2C_AllPlayerMovePacket allPlayerStatePacket{};
	allPlayerStatePacket.size = sizeof(common::packet::S2C_AllPlayerMovePacket);
	allPlayerStatePacket.type = common::packet::PacketType::AllPlayerMovePacket_s2c;

	
	{
		std::lock_guard lock(_playerMutex);
        // MAX_PLAYERS만큼 순회하며 활성화된 플레이어 정보만 패킷에 담음
		for (int i = 0; i < MAX_PLAYERS; ++i)
		{
            if (Players[i].isActive) // 활성 상태인 플레이어만 처리
            {
                size_t id = Players[i].id; // id와 인덱스가 일치하므로
                allPlayerStatePacket.x[id] = Players[i].x;
                allPlayerStatePacket.y[id] = Players[i].y;
                allPlayerStatePacket.state[id] = Players[i].state;
                allPlayerStatePacket.Rotate[id] = Players[i].Rotate;
            } else { // 비활성 플레이어는 상태를 PLAYER_STATE_END로 설정하여 클라이언트에 알림
                allPlayerStatePacket.x[i] = 0; // 또는 기본값
                allPlayerStatePacket.y[i] = 0;
                allPlayerStatePacket.state[i] = common::packet::Player_State::PLAYER_STATE_END;
                allPlayerStatePacket.Rotate[i] = 0;
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
        printf("[경고] 알 수 없는 패킷 타입 수신: %d\n", static_cast<int>(header->type));
        // session->Disconnect(); // 알 수 없는 패킷 수신 시 연결 끊기 (선택 사항)
	}
}

void Room::MovePacket_c2s(Session* session, char* packet)
{
	common::packet::C2S_MovePacket* movePacket = reinterpret_cast<common::packet::C2S_MovePacket*>(packet);
	
	if (movePacket->id >= 0 && movePacket->id < MAX_PLAYERS && Players[movePacket->id].isActive)
	{
		// 세션 포인터를 통해 해당 플레이어의 상태를 직접 업데이트
		Player& player = Players[movePacket->id];
		player.x = movePacket->x;
		player.y = movePacket->y;
		player.state = movePacket->player_state;
		player.Rotate = movePacket->rotate;
	} else {
        printf("[경고] MovePacket_c2s - 유효하지 않거나 비활성화된 플레이어 ID: %d\n", movePacket->id);
    }
}
