#pragma once
#include "Default.h"





#define BUFFER_SIZE 4096
class ClientPacketManager;
class NetworkManager : public Singleton<NetworkManager>
{
private:
	friend class Singleton<NetworkManager>;

	NetworkManager();
	~NetworkManager();

public:
	/*void GetInstance();
	void DestroyInstance();*/

	bool initialize_Client(); // 서버 접속 및 논블로킹 소켓 설정
	void sendPacket(char* buffer, int size); // 서버로 패킷 전송 <- Main Thread에서 호출
	void trySendBuffer();
	void processPacket(); // 서버로 부터 패킷 처리 
	void updatePacket(); //recv -> send
	void shutdown(); // 접속 종료

private:

	ClientPacketManager* _packetManager; // 패킷 라우터
	SOCKET _clientSocket;				 // 서버 연결 소켓

	char _recvBuffer[BUFFER_SIZE];		 // 고정 크기 버퍼
	unsigned long long _recvBufferSize;  // 현재 버퍼에 쌓인 데이터 크기

	std::vector<char> _sendBuffer;		 // 동적 송신 버퍼
	size_t _sendBufferOffset = 0;		 // 현재까지 보낸 위치
};

