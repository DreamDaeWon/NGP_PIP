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

	// DW추가 : 클라이언트 id 알려주는 함수
	void sendClientID(int clientID);

	// DW추가 : 모든 클라이언트에게 다른 플레이어 알려주는 함수
	//			새로운 클라이언트에게는 기존에 존재하는 플레이어들도 알려줘야 함
	void broadcastNewPlayer(int newPlayerID);

public:
	Clients _clients;
private:
	SOCKET _listenSocket = INVALID_SOCKET;
	uint32_t _playerCount = 0;
	Room _room;
	std::thread _roomThread;
};
