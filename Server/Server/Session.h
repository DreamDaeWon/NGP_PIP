#pragma once
#include "Player.h"

enum class ClientState
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

	Session(const Session& other);
	Session& operator=(const Session& other);


	~Session();

	void WorkerLoop();
	void ProcessPacket();
	void SendPacket();
	void RecvPacket();
	void Disconnect();

	uint32_t getId() const { return _id; }
private:
	uint32_t _id;
	SOCKET _socket;
	ClientState _state;
	Player _player;
	std::array<char, 1024> _recvBuffer;
	std::array<char, 1024> _sendBuffer;

	concurrency::concurrent_queue<Common::packet::PacketHeader*> sendQueue;
};
