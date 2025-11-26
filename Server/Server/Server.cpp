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

		// 소켓을 논블로킹 모드로 설정합니다.
		u_long mode = 1; // 1 for non-blocking
		if (ioctlsocket(clientsocket, FIONBIO, &mode) == SOCKET_ERROR)
		{
			err_display("ioctlsocket() non-blocking failed");
			closesocket(clientsocket);
			continue;
		}

		uint32_t idx = _playerCount; // 0-based
		_clients._sessionLock.lock();
		_clients._sessions[idx].init(idx, clientsocket, ClientState::Connected);
		_clients._sessions[idx].setCurrentRoom(&_room);
		_clients._sessionLock.unlock();

		_room.AddPlayer();

		// DW추가 : 여기에 플레이어 ID 전송해주는 함수 추가함

		_playerCount++;

		// 스레드를 분리하여 독립적으로 실행되게 합니다.
		// 소멸 시 자동으로 리소스를 해제하므로 수동 관리가 필요 없습니다.
		std::thread worker{ &Session::WorkerLoop, &_clients._sessions[idx] };
		worker.detach();

		sendClientID((int)idx);
		broadcastNewPlayer((int)idx);

		// 모든 클라이언트에게 다른 플레이어 알려주기

	}

}

void Server::sendClientID(int clientID)
{
	// 1. Create packet on stack
	common::packet::S2C_LoginAcceptPacket packet;
	packet.type = common::packet::PacketType::LoginAcceptPacket_s2c;
	packet.size = sizeof(common::packet::S2C_LoginAcceptPacket);
	packet.playerID = clientID;

	printf("플레이어 ID알려줌 : %d\n", clientID);

	// 2. Create a shared vector from the packet data
	auto packetData = std::make_shared<std::vector<char>>(packet.size);
	memcpy(packetData->data(), &packet, packet.size);

	// 3. Enqueue the packet for sending
	_clients._sessions[clientID].EnqueuePacket(packetData);
}

void Server::broadcastNewPlayer(int newPlayerID)
{
	// 1. Create packet on stack
	common::packet::S2C_SpawnOtherPlayerPacket packet;
	packet.type = common::packet::PacketType::SpawnOtherPlayerPacket_s2c;
	packet.size = sizeof(common::packet::S2C_SpawnOtherPlayerPacket);
	packet.OtherplayerID = newPlayerID;
	printf("새 플레이어 알림 : %d\n", newPlayerID);
	// 2. Create a shared vector from the packet data
	auto packetData = std::make_shared<std::vector<char>>(packet.size);
	memcpy(packetData->data(), &packet, packet.size);
	// 3. Enqueue the packet for sending to all connected clients
	std::lock_guard<std::mutex> lock(_clients._sessionLock);
	for (auto& session : _clients._sessions)
	{
		if (session.isConnected())
		{
			session.EnqueuePacket(packetData);
		}
	}

	// 기존 플레이어들도 새로운 플레이어에게 알려주기
	for (uint32_t id = 0; id < _playerCount; ++id)
	{
		if (id == newPlayerID) continue; // 자기 자신은 제외
		common::packet::S2C_SpawnOtherPlayerPacket existingPlayerPacket;
		existingPlayerPacket.type = common::packet::PacketType::SpawnOtherPlayerPacket_s2c;
		existingPlayerPacket.size = sizeof(common::packet::S2C_SpawnOtherPlayerPacket);
		existingPlayerPacket.OtherplayerID = id;
		auto existingPacketData = std::make_shared<std::vector<char>>(existingPlayerPacket.size);
		memcpy(existingPacketData->data(), &existingPlayerPacket, existingPlayerPacket.size);
		_clients._sessions[newPlayerID].EnqueuePacket(existingPacketData);
	}
}
