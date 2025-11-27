#include "NetworkHeader.h"
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
		err_quit("WSAStartup()");
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
		err_quit("socket()");
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
		err_quit("connect()");
		return false;
	}

	// CJ 메모 : 논블로킹 소켓 설정 -> 이제부터 recv가 블로킹 되지 않음
	u_long mode = 1;
	if (SOCKET_ERROR == ioctlsocket(_clientSocket, FIONBIO, &mode))
	{
		closesocket(_clientSocket);
		_clientSocket = INVALID_SOCKET;
		err_display("ioctlsocket()");
		return false;
	}

	_recvBufferSize = 0;

	common::packet::C2S_LoginRequestPacket login_request_packet;

	login_request_packet.size = sizeof(login_request_packet);
	login_request_packet.type = common::packet::PacketType::LoginRequestPacket_c2s;

	sendPacket(reinterpret_cast<char*>(&login_request_packet), sizeof(login_request_packet));

	return true;
}

void NetworkManager::sendPacket(char* buffer, int size)
{
	if (_clientSocket == INVALID_SOCKET)
	{
		err_display("sendPacket() - Invalid socket");
		return;
	}

	// 버퍼 크기 검증
	if (size <= 0 || size > 4096) // 최대 패킷 크기 제한
	{
		err_display("sendPacket() - Invalid packet size");
		return;
	}

	// 송신 버퍼 크기 제한 (메모리 보호)
	if (_sendBuffer.size() > 65536) // 64KB 제한
	{
		err_display("sendPacket() - Send buffer overflow");
		return;
	}

	// 보낼 데이터를 송신 버퍼에 추가
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
				// 송신 버퍼가 가득 참, 잠시 후 다시 시도
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
	// 모든 데이터를 보냈으면 초기화
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
		// 1. 버퍼에서 패킷의 전체 크기를 확인
		PacketHeader* pHeader = (PacketHeader*)_recvBuffer;
		unsigned long long packetSize = pHeader->size;

		// 2. 버퍼에 패킷 전체 크기만큼의 데이터가 있는지 확인
		if (_recvBufferSize >= packetSize)
		{
			// 3. ClientPacketManager에게 패킷 처리 요청
			if (_packetManager)
			{
				_packetManager->HandlePacket(_recvBuffer);
			}

			// 4. 버퍼에서 처리된 패킷만큼 제거
			unsigned long long remainingData = _recvBufferSize - packetSize;
			if (remainingData > 0)
			{
				// 뒤에 남은 데이터를 앞으로 당김 (memmove)
				memmove(_recvBuffer, _recvBuffer + packetSize, remainingData);
			}
			_recvBufferSize = remainingData; // 버퍼 크기 갱신
		}
		else
		{
			// 5. 패킷 전체가 아직 도착하지 않음 (다음 recv 대기)
			break;
		}
	}
}

void NetworkManager::updatePacket()
{
	if (_clientSocket == INVALID_SOCKET) return;

	// 버퍼가 가득 찼는지 확인 (안전장치)
	if ((int)(BUFFER_SIZE - _recvBufferSize) <= 0)
	{
		printf("[오류] 수신 버퍼가 가득 찼습니다. 패킷 처리가 지연되고 있거나 패킷 크기가 너무 큽니다.\n");
		return;
	}

	// 1. _recvBuffer에 남은 공간에 데이터를 수신 시도
	int bytesRecv = recv(_clientSocket,
		_recvBuffer + _recvBufferSize,   // 저장할 버퍼 위치
		(int)(BUFFER_SIZE - _recvBufferSize),          // 저장할 수 있는 크기
		0);

	if (bytesRecv > 0)
	{
		// 2. 데이터가 들어옴, 버퍼 크기를 늘림
		_recvBufferSize += bytesRecv;

		// 3. 버퍼에 완성된 패킷이 있는지 파싱
		processPacket();
	}
	else if (bytesRecv == 0)
	{
		// 4. (연결 종료) 서버가 연결을 끊음
		printf("서버와 연결이 종료되었습니다.\n");
		shutdown();
	}
	else // bytesRecv == SOCKET_ERROR
	{
		int error = WSAGetLastError();
		if (error == WSAEWOULDBLOCK)
		{
			// 5. (대기) 데이터가 아직 없음. 정상적인 논블로킹 상태.
			// processPacket()은 데이터가 새로 들어왔을 때만 호출하면 됨.
			return;
		}
		else
		{
			// 6. (에러) 진짜 에러 발생.
			err_display("recv()");
			shutdown();
		}
	}

	trySendBuffer(); // send할 것이 있다면 시도
}

void NetworkManager::shutdown()
{
	if (_clientSocket != INVALID_SOCKET)
	{
		closesocket(_clientSocket);
		_clientSocket = INVALID_SOCKET;
	}
}
