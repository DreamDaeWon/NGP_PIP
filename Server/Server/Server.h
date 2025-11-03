#pragma once
#include "Session.h"

constexpr uint32_t MAX_PLAYERS = 3;

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
};
