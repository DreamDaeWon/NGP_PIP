#include "NetworkManager.h"
#include "ClientPacketManager.h"
#include <ws2tcpip.h>

using namespace common::packet;

NetworkManager::NetworkManager()
	:_serverSocket(INVALID_SOCKET),
	_recvBufferSize(0)
{
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		// 에러	
	}

	_packetManager = ClientPacketManager::Instance();
}

NetworkManager::~NetworkManager()
{
	shutdown();
	WSACleanup();
}

void NetworkManager::GetInstance()
{
}

void NetworkManager::DestroyInstance()
{
}

bool NetworkManager::initialize_Client(const char* ip, unsigned short port)
{
	_serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (_serverSocket == INVALID_SOCKET)
	{
		// TODO: 소켓 생성 실패	
		return false;
	}

	sockaddr_in server_address;
	memset(&server_address, 0, sizeof(server_address));
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(port);
	inet_pton(AF_INET, ip, &server_address.sin_addr);

	// 서버 접속
	if (connect(_serverSocket, (sockaddr*)&server_address, sizeof(server_address)) == SOCKET_ERROR)
	{
		closesocket(_serverSocket);
		_serverSocket = INVALID_SOCKET;
		return false;
	}

	// CJ 설명 : 논블로킹 소켓 설정 -> 게임 루프가 recv에서 멈추지 않도록 함
	u_long mode = 1;
	if (SOCKET_ERROR == ioctlsocket(_serverSocket, FIONBIO, &mode))
	{
		closesocket(_serverSocket);
		_serverSocket = INVALID_SOCKET;
		return false;
	}

	_recvBufferSize = 0;

	// 1. 패킷을 담을 버퍼 생성
	const int packetSize = sizeof(PacketHeader) + sizeof(PacketType::LoginRequestPacket_c2s);
	char loginPacketBuffer[packetSize];

	// 2. 헤더 채우기
	PacketHeader* header = (PacketHeader*)loginPacketBuffer;
	header->size = packetSize;
	header->type = PacketType::LoginRequestPacket_c2s;

	// TODO (나중에 ID/PW 채우기)
	// LoginRequestPacket_c2s* payload = (LoginRequestPacket_c2s*)(loginPacketBuffer + sizeof(PacketHeader));
	// payload->userID = ... (만약 ID/PW가 있다면)

	sendPacket(loginPacketBuffer, packetSize);

	return true;
}

void NetworkManager::sendPacket(char* buffer, int size)
{
	if (_serverSocket != INVALID_SOCKET)
	{
		send(_serverSocket, buffer, size, 0);
	}
}

void NetworkManager::processPacket()
{
	while (_recvBufferSize >= sizeof(PacketHeader))
	{
		// 1. 헤더를 읽어 패킷의 전체 크기를 확인
		PacketHeader* pHeader = (PacketHeader*)_recvBuffer;
		long long packetSize = pHeader->size;

		// 2. 버퍼에 패킷 전체 크기만큼의 데이터가 쌓였는지 확인
		if (_recvBufferSize >= packetSize)
		{
			// 3. ClientPacketManager에게 버퍼를 넘겨 처리 요청
			if (_packetManager)
			{
				_packetManager->HandlePacket(_recvBuffer);
			}

			// 4. 버퍼에서 처리된 패킷만큼 제거
			int remainingData = _recvBufferSize - packetSize;
			if (remainingData > 0)
			{
				// 뒤에 남은 데이터를 버퍼 앞으로 당겨옴 (memmove)
				memmove(_recvBuffer, _recvBuffer + packetSize, remainingData);
			}
			_recvBufferSize = remainingData; // 버퍼 크기 갱신
		}
		else
		{
			// 5. recv()를 위해 루프 종료
			break;
		}
	}
}

void NetworkManager::updatePacket()
{
	if (_serverSocket == INVALID_SOCKET) return;

	// 1. _recvBuffer의 남은 공간에 데이터를 수신 시도
	int bytesRecv = recv(_serverSocket,
		_recvBuffer + _recvBufferSize,   // 버퍼의 빈 공간 시작 위치
		BUFFER_SIZE - _recvBufferSize,          // 버퍼의 남은 크기
		0);

	if (bytesRecv > 0)
	{
		// 2. 데이터가 왔으면, 버퍼에 쌓인 크기를 늘림
		_recvBufferSize += bytesRecv;

		// 3. 버퍼에 완성된 패킷이 있는지 파싱
		processPacket();
	}
	else if (bytesRecv == 0)
	{
		// 4. (접속 종료) 서버가 접속을 끊음
		shutdown();
	}
	else // bytesRecv == SOCKET_ERROR
	{
		int error = WSAGetLastError();
		if (error == WSAEWOULDBLOCK)
		{
			// 5. (정상) 데이터가 없는 것뿐. 아무것도 하지 않음.
		}
		else
		{
			// 6. (오류) 실제 오류 발생. 접속 종료.
			shutdown();
		}
	}
}

void NetworkManager::shutdown()
{
	if (_serverSocket != INVALID_SOCKET)
	{
		closesocket(_serverSocket);
		_serverSocket = INVALID_SOCKET;
	}
}
