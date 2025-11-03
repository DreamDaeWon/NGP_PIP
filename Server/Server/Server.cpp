#include "pch.h"
#include "Server.h"

#include "Session.h"

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
	serveraddr.sin_port = htons(SERVERPORT);
	if (SOCKET_ERROR == bind(_listenSocket, (sockaddr*)&serveraddr, sizeof(serveraddr)))
	{
		err_quit("bind() 오류");
	}
}

void Server::start()
{
	if (SOCKET_ERROR == listen(_listenSocket, SOMAXCONN))
	{
		err_quit("listen() 오류");
	}
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
	for (auto& worker : _workers)
	{
		if (worker.joinable())
			worker.join();
	}
	_workers.clear();

	for (auto& session : _sessions)
	{
		session.Disconnect();
	}

	WSACleanup();
}

void Server::acceptLoop()
{
	while (true)
	{
		if (_playerCount >= MAX_PLAYERS)
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

		uint32_t idx = _playerCount; // 0-based
		_sessions[idx] = Session(idx + 1, clientsocket, ClientState::Connected);
		_playerCount++;

		_workers.emplace_back(&Session::WorkerLoop, &_sessions[idx]);

	}

}
