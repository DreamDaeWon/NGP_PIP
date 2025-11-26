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
	

	ClientPacketManager* _packetManager;
	SOCKET _clientSocket;

	char _recvBuffer[BUFFER_SIZE];
	unsigned long long _recvBufferSize;

	std::vector<char> _sendBuffer;
	size_t _sendBufferOffset = 0;
};

