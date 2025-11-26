#pragma once
#include "Room.h"
#include "Session.h"
#include "Singleton.h"
#include <queue> // std::priority_queue 사용을 위해 추가
#include <vector> // std::priority_queue에 필요

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

    // 사용이 끝난 클라이언트 ID를 재사용 큐에 반환하는 함수 추가
    void ReturnClientID(int clientID);

public:
	Clients _clients;
private:
	SOCKET _listenSocket = INVALID_SOCKET;
    std::priority_queue<int, std::vector<int>, std::greater<int>> _freeIds; // 사용 가능한 ID를 저장 (min-heap)
	Room _room;
	std::thread _roomThread;
};