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
	serveraddr.sin_port = htons(SERVER_PORT);
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
	for (auto& worker : _workers)
	{
		if (worker.joinable())
			worker.join();
	}
	if (_roomThread.joinable())
	{
		_roomThread.join();
	}
	_workers.clear();

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
		_clients._sessionLock.lock();
		_clients._sessions[idx].init(idx, clientsocket, ClientState::Connected);
		_clients._sessions[idx].setCurrentRoom(&_room);
		_clients._sessionLock.unlock();

		_room.AddPlayer();

		// DW추가 : 여기에 플레이어 ID 전송해주는 함수 추가함
		sendClientID(idx);

		_playerCount++;

		_workers.emplace_back(&Session::WorkerLoop, &_clients._sessions[idx]);

	}

}

void Server::sendClientID(int clientID)
{
	// 1. 패킷 생성
	common::packet::S2C_LoginAcceptPacket* packet = new common::packet::S2C_LoginAcceptPacket();

	// 2. 헤더 설정 (생성자에서 안 한다면 필수!)
	// PacketType enum에 해당하는 타입이 정의되어 있어야 합니다.
	packet->type = common::packet::PacketType::LoginAcceptPacket_s2c;
	packet->size = sizeof(common::packet::S2C_LoginAcceptPacket);

	// 3. 데이터 설정 (uint32_t -> int 형변환)
	packet->playerID = clientID;
	printf("플레이어 ID알려줌 : %d\n", clientID);

	// 4. 세션 큐에 전송 예약
	// 워커 스레드가 돌기 시작하면 바로 전송됩니다.
	_clients._sessions[clientID].EnqueuePacket(reinterpret_cast<common::packet::PacketHeader*>(packet));
}
