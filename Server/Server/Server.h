#pragma once
#include "Room.h"
#include "Session.h"



class Server
{
public:
	void initailize();
	void start();
	void run();
	void end();
	void acceptLoop();
private:
	SOCKET _listenSocket = INVALID_SOCKET;
	uint32_t _playerCount = 0;
	std::array<Session, 3> _sessions;
	std::vector<std::thread> _workers;
	Room _room;
	std::thread _roomThread;
};
