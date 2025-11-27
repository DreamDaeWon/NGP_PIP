#include "pch.h"
#include "Server.h"

#include "Session.h"
#include <algorithm> // std::find_if 사용을 위해 추가

void Server::initailize()
{
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		err_quit("WSA StartUp 오류");

	_listenSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (INVALID_SOCKET == _listenSocket)
	{
		err_quit("서버 소켓 생성 오류");
	}

	sockaddr_in serveraddr;
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVER_PORT);
	if (SOCKET_ERROR == bind(_listenSocket, (sockaddr*)&serveraddr, sizeof(serveraddr)))
	{
		err_quit("bind() 오류");
	}

    // 초기화 시 모든 ID를 _freeIds 큐에 추가 (0부터 MAX_PLAYERS-1까지)
    for (int i = 0; i < MAX_PLAYERS; ++i)
    {
        _freeIds.push(i);
    }
}

void Server::start()
{
	if (SOCKET_ERROR == listen(_listenSocket, SOMAXCONN))
	{
		err_quit("listen() 오류");
	}
	_room.StartGame();
	_roomThread = std::thread(&Room::UpdateGame, &_room);
	// ToDo : 패킷 매니저 등 초기화 여기서 진행
}

void Server::run()
{
	acceptLoop();
}

void Server::end()
{
	if (INVALID_SOCKET != _listenSocket)
	{
		closesocket(_listenSocket);
		_listenSocket = INVALID_SOCKET;
	}
	if (_roomThread.joinable())
	{
		_roomThread.join();
	}

	for (auto & session : _clients._sessions)
	{
		session.Disconnect();
	}
	WSACleanup();
}

void Server::acceptLoop()
{
	while (true)
	{
        // _freeIds 큐가 비어있으면 (사용 가능한 ID가 없으면) 새 클라이언트 접속을 받지 않음
		if (_freeIds.empty())
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
			continue;
		}
		sockaddr_in clientaddr;
		int addrlen = sizeof(clientaddr);
		SOCKET clientsocket = accept(_listenSocket, (sockaddr*)&clientaddr, &addrlen);
		if (INVALID_SOCKET == clientsocket)
		{
			int err = WSAGetLastError();
			// listen 소켓이 닫혀서 accept가 깨졌으면 루프 종료
			if (_listenSocket == INVALID_SOCKET)
				break;

			// 일시적 오류라면 계속
			err_display("accept() 오류");
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
			continue;
		}
		char addr[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &clientaddr.sin_addr, addr, INET_ADDRSTRLEN);
		printf("클라이언트 접속: %s:%d\n",
			addr,
			ntohs(clientaddr.sin_port));

		// 소켓을 논블로킹 모드로 설정합니다.
		u_long mode = 1; // 1 for non-blocking
		if (ioctlsocket(clientsocket, FIONBIO, &mode) == SOCKET_ERROR)
		{
			err_display("ioctlsocket() non-blocking failed");
			closesocket(clientsocket);
			continue;
		}

		int flag = 1;
		if (setsockopt(clientsocket, IPPROTO_TCP, TCP_NODELAY, (char*)&flag, sizeof(flag)) == SOCKET_ERROR)
		{
			err_display("TCP_NODELAY failed");
		}


        // _freeIds에서 사용 가능한 가장 작은 ID를 가져옴
        int clientID = _freeIds.top();
        _freeIds.pop();

		_clients._sessionLock.lock();
		_clients._sessions[clientID].init(clientID, clientsocket, ClientState::Connected);
		_clients._sessions[clientID].setCurrentRoom(&_room);
		_clients._sessionLock.unlock();

        // Room에 플레이어 추가 (내부에서 SendClientID, BroadcastNewPlayer 호출)
		_room.AddPlayer(clientID); 

		// 스레드를 분리하여 독립적으로 실행되게 합니다.
		// 소멸 시 자동으로 리소스를 해제하므로 수동 관리가 필요 없습니다.
		std::thread worker{ &Session::WorkerLoop, &_clients._sessions[clientID] };
		worker.detach();

        // sendClientID, broadcastNewPlayer 호출 삭제됨 (Room::AddPlayer 내부로 이동)
	}
}

// 사용이 끝난 클라이언트 ID를 _freeIds 큐에 반환
void Server::ReturnClientID(int clientID)
{
    if (clientID >= 0 && clientID < MAX_PLAYERS)
    {
        _freeIds.push(clientID);
        printf("클라이언트 ID %d 반환\n", clientID);
    }
}