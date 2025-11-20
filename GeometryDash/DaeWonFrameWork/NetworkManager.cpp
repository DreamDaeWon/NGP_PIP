#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32") // ws2_32.lib 링크

#include "NetworkManager.h"
#include "ClientPacketManager.h"

using namespace common::packet;

NetworkManager::NetworkManager()
	:_clientSocket(INVALID_SOCKET),
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

//void NetworkManager::GetInstance()
//{
//}
//
//void NetworkManager::DestroyInstance()
//{
//}

bool NetworkManager::initialize_Client()
{
	_clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (_clientSocket == INVALID_SOCKET)
	{
		// TODO: 소켓 생성 실패
		// TODO : 에러 로그 남기기
		return false;
	}

	sockaddr_in server_address {};
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(SERVER_PORT);
	inet_pton(AF_INET, SERVER_IP, &server_address.sin_addr);

	// 서버 접속
	if (connect(_clientSocket, (sockaddr*)&server_address, sizeof(server_address)) == SOCKET_ERROR)
	{
		closesocket(_clientSocket);
		_clientSocket = INVALID_SOCKET;
		return false;
	}

	// CJ 설명 : 논블로킹 소켓 설정 -> 게임 루프가 recv에서 멈추지 않도록 함
	// CJ 설명 일단 따라함 정확한	이해 필요
	u_long mode = 1;
	if (SOCKET_ERROR == ioctlsocket(_clientSocket, FIONBIO, &mode))
	{
		closesocket(_clientSocket);
		_clientSocket = INVALID_SOCKET;
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

	sendPacket(loginPacketBuffer, packetSize);

	return true;
}

void NetworkManager::sendPacket(char* buffer, int size)
{
	if (_clientSocket == INVALID_SOCKET)
		return;

	// 남은 데이터가 있으면 버퍼에 추가
	_sendBuffer.insert(_sendBuffer.end(), buffer, buffer + size);
	trySendBuffer();
}

void NetworkManager::trySendBuffer()
{
	while (_sendBufferOffset < _sendBuffer.size())
	{
		int toSend = (int)(_sendBuffer.size() - _sendBufferOffset);
		int sent = send(_clientSocket, &_sendBuffer[_sendBufferOffset], toSend, 0);
		if (sent > 0)
		{
			_sendBufferOffset += sent;
		}
		else if (sent == SOCKET_ERROR)
		{
			int error = WSAGetLastError();
			if (error == WSAEWOULDBLOCK)
			{
				// 다음 루프에서 다시 시도
				break;
			}
			else
			{
				shutdown();
				break;
			}
		}
		else
		{
			shutdown();
			break;
		}
	}
	// 모두 보냈으면 버퍼 초기화
	if (_sendBufferOffset >= _sendBuffer.size())
	{
		_sendBuffer.clear();
		_sendBufferOffset = 0;
	}
}

void NetworkManager::processPacket()
{
	while (_recvBufferSize >= sizeof(PacketHeader))
	{
		// 1. 헤더를 읽어 패킷의 전체 크기를 확인
		PacketHeader* pHeader = (PacketHeader*)_recvBuffer;
		unsigned long long packetSize = pHeader->size;

		// 2. 버퍼에 패킷 전체 크기만큼의 데이터가 쌓였는지 확인
		if (_recvBufferSize >= packetSize)
		{
			// 3. ClientPacketManager에게 버퍼를 넘겨 처리 요청
			if (_packetManager)
			{
				_packetManager->HandlePacket(_recvBuffer);
			}

			// 4. 버퍼에서 처리된 패킷만큼 제거
			unsigned long long remainingData = _recvBufferSize - packetSize;
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
	if (_clientSocket == INVALID_SOCKET) return;

	// 1. _recvBuffer의 남은 공간에 데이터를 수신 시도
	int bytesRecv = recv(_clientSocket,
		_recvBuffer + _recvBufferSize,   // 버퍼의 빈 공간 시작 위치
		(int)(BUFFER_SIZE - _recvBufferSize),          // 버퍼의 남은 크기
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
			return;
		}
		else
		{
			// 6. (오류) 실제 오류 발생. 접속 종료.
			shutdown();
		}
	}

	trySendBuffer(); // send도 주기적으로 시도
}

void NetworkManager::shutdown()
{
	if (_clientSocket != INVALID_SOCKET)
	{
		closesocket(_clientSocket);
		_clientSocket = INVALID_SOCKET;
	}
}
