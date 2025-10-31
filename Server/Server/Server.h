#pragma once

class Server
{
	SOCKET listenSocket = INVALID_SOCKET;
public:
	void initailize();
	void start();
	void run();
	void end();
	void acceptLoop();
};
