#pragma once
class NetworkManager
{
public:
	NetworkManager();
	~NetworkManager();

public:
	void GetInstance();
	void DestroyInstance();

	void initialize_Client();
	void sendPacket();
	void recevPacket();
	void updatePacket();
	void shutdown();
};

