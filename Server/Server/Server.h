#pragma once
#include "Room.h"
#include "Session.h"
#include "Singleton.h"
class Clients
{
public:
	std::mutex				_sessionLock;
	std::array<Session, 3>	_sessions;
};


class Server : public Singleton<Server>
{
	friend class Singleton<Server>;
public:
	void initailize();
	void start();
	void run();
	void end();
	void acceptLoop();
public:
	Clients _clients;
private:
	SOCKET _listenSocket = INVALID_SOCKET;
	uint32_t _playerCount = 0;
	std::vector<std::thread> _workers;
	Room _room;
	std::thread _roomThread;
};
