#pragma once
#include "pch.h"
#include <memory>
#include "Player.h"
#include "Packet.h"
// DW예정 : 추후에 추가되면 추가할 예정인 헤더들

enum class RoomGameMode
{
	SinglePlayer,
	MutiPlayer,
	ROOM_MODE_MAX
};

// DW질문 : Player 구조제 관련 애들 어디에 정의해두어야 하지?
// KJ 해답 : 파일하나 만들어서 Player.h로 정의해두는게 좋을듯


// KJ 질문 : 이게 뭐임
// DW 답변 : 브로드 캐스트밖에 안할꺼니까 묶어서 보내자, 이동 패킷을 바꾸자
// KJ 해답 : 아하 알겠음

class Session; // DW설명 : 전방 선언
class Timer;
using PacketHandlerFunc = std::function<void(Session*, char*)>;
class Room
{
public:
	Room();
	~Room();

public:
	// DW예정 : 리턴 타입 및 매개변수 구현 때 하나 씩 수정할 예정
	void AddPlayer();
	void RemovePlayer(int id);
	void StartGame();
	void StopGame();
	void UpdateGame();
	void BroadcastPacket(common::packet::PacketHeader* packet);
	void EnqueuePacket(std::unique_ptr<common::packet::PacketHeader> packet);

	void RegisterHandler(common::packet::PacketType type, PacketHandlerFunc func);
	void RegisterHandler(common::packet::PacketType type, void(Room::* func)(Session*, char*));
	void HandlePacket(Session* session, char* packet);

	void MovePacket_c2s(Session* session, char* packet);

private:
	void ProcessInputs();
	void BroadcastState();


private:
	long long CurrentMapSize{};
	std::vector<Player> Players{};
	std::mutex _playerMutex;

	Timer* timer = nullptr;

	std::queue<std::unique_ptr<common::packet::PacketHeader>> incomingQueue;

	RoomGameMode mode{ RoomGameMode::ROOM_MODE_MAX };

	std::mutex _queueMutex;
	std::atomic_bool _isGameRunning{ false };

	std::unordered_map<common::packet::PacketType, PacketHandlerFunc> Handlers;
};