#pragma once
#include "Player.h"
#include <vector>
#include <memory>
#include <queue>

class Room; // 전방 선언

enum class ClientState : int
{
	Connected,
	InRoom,
	InGame,
	Disconnected
};


class Session
{
public:
	Session();
	Session(uint32_t id, SOCKET socket, ClientState state);

	Session(const Session& other) = delete;
	Session& operator=(const Session& other) = delete;


	~Session();
	void init(uint32_t id, SOCKET socket, ClientState state);
	bool isConnected() const { return _state != ClientState::Disconnected; }
	ClientState getState() const { return _state; }
	void WorkerLoop();
	void SendPacket();
	void Disconnect();

	void EnqueuePacket(std::shared_ptr<std::vector<char>> packet);


	int getId() const { return _id; }

	void setCurrentRoom(Room* room) { _currentRoom = room; }
private:
	void DoRecv();

	int _id;
	SOCKET _socket;
	std::atomic<ClientState> _state;
	Room* _currentRoom;

	std::array<char, 1024> _recvBuffer;
	size_t _recvBufferOffset = 0;
	std::array<char, 1024> _sendBuffer;

	std::mutex _sendMutex;
	std::queue<std::shared_ptr<std::vector<char>>> _sendQueue;
};