#include "pch.h"
#include "Server.h"

int main()
{
	Server server;
	server.initailize();
	server.start();
	server.run();
	server.end();
	return 0;
}
