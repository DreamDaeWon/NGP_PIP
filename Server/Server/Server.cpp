#include "pch.h"
#include "Server.h"

void Server::initailize()
{
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		err_quit("WSA StartUp 오류");

	listenSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (INVALID_SOCKET == listenSocket)
	{
		err_quit("서버 소켓 생성 오류");
	}

	sockaddr_in serveraddr;
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVERPORT);
	if (SOCKET_ERROR == bind(listenSocket, (sockaddr*)&serveraddr, sizeof(serveraddr)))
	{
		err_quit("bind() 오류");
	}
}

void Server::start()
{
}

void Server::run()
{
}

void Server::end()
{
}

void Server::acceptLoop()
{
}
