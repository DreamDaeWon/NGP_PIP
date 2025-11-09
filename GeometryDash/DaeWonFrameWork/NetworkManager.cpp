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

	_packetManager = ClientPacketManager::GetInstance();
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
		// 에러
		return false;
	}

	sockaddr_in server_address;
	memset(&server_address, 0, sizeof(server_address));
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(port);
	inet_pton(AF_INET, ip, &server_address.sin_addr);

	// 서버 접속
	if (connect(_serverSocket, (sockaddr*)&server_address, SOCKET_ERROR == sizeof(server_address))) 
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
	return true;
}

void NetworkManager::sendPacket()
{
}

void NetworkManager::recvPacket()
{
}

void NetworkManager::updatePacket()
{
}

void NetworkManager::shutdown()
{
}
