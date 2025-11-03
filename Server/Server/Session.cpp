#include "pch.h"
#include "Session.h"

Session::Session() : _id(0), _socket(INVALID_SOCKET), _state(ClientState::Disconnected)
{
}

Session::Session(uint32_t id, SOCKET socket, ClientState state) : _id(id), _socket(socket), _state(state)
{
}

Session::Session(const Session& other)
{
	if (this != &other)
	{
		_id = other._id;
		_socket = other._socket;
		_state = other._state;
		_player = other._player;
	}
}

Session& Session::operator=(const Session& other)
{
	if (this != &other)
	{
		_id = other._id;
		_socket = other._socket;
		_state = other._state;
		_player = other._player;
	}
	return *this;
}

Session::~Session()
{
}

void Session::WorkerLoop()
{
}

void Session::ProcessPacket()
{
}

void Session::SendPacket()
{
}

void Session::RecvPacket()
{
}

void Session::Disconnect()
{
}
