#pragma once
#include "Singleton.h"
#include "Packet.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32") // ws2_32.lib 링크


#define BUFFER_SIZE 4096
class ClientPacketManager;
class NetworkManager : public Singleton<NetworkManager>
{
private:
	NetworkManager();
	~NetworkManager();

public:
	void GetInstance();
	void DestroyInstance();

	bool initialize_Client(const char* ip, unsigned short port); // 서버 접속 및 논블로킹 소켓 설정
	void sendPacket(char* buffer, int size); // 서버로 패킷 전송 <- Main Thread에서 호출
	void processPacket(); // 서버로 부터 패킷 처리
	void updatePacket(); // 게임 루프에서 매 프레임 호출 서버로부터 온 데이터를 수신하고 큐에 넣음 
	void shutdown(); // 접속 종료

private:
	friend class Singleton<NetworkManager>;

	ClientPacketManager* _packetManager;
	SOCKET _clientSocket;

	char _recvBuffer[BUFFER_SIZE];
	long long _recvBufferSize;
};

