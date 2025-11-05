#include "pch.h"
#include "Session.h"

#include "ServerPacketManager.h"

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
	while (_state != ClientState::Disconnected)
	{
		RecvPacket();
		ProcessPacket();
		SendPacket();
	}
}

void Session::ProcessPacket()
{
	// 패킷 처리 로직 구현
	// 예: 패킷 헤더 파싱, 플레이어 상태 업데이트 등
	// _recvBuffer를 사용하여 받은 데이터를 처리
	// TODO: 받은 데이터 패킷으로 재조립
	// TODO: 패킷 타입에 따라 적절한 처리 수행
	using namespace Common::packet;
	Common::packet::PacketHeader* header = reinterpret_cast<Common::packet::PacketHeader*>(_recvBuffer.data());
	PacketHeader* deserializedPacket = nullptr;
	switch (header->type)
	{
	case PacketType::LoginRequestPacket_c2s:
		deserializedPacket = new Common::packet::C2S_LoginAcceptPacket();
		memcpy(deserializedPacket, _recvBuffer.data(), sizeof(Common::packet::C2S_LoginAcceptPacket));
		break;
	case PacketType::LogoutPacket_c2s:
		deserializedPacket = new Common::packet::C2S_LoginAcceptPacket();
		memcpy(deserializedPacket, _recvBuffer.data(), sizeof(Common::packet::C2S_LoginAcceptPacket));
		break;
	case PacketType::RoomEnterPacket_c2s:
		deserializedPacket = new Common::packet::C2S_RoomEnterAcceptPacket();
		memcpy(deserializedPacket, _recvBuffer.data(), sizeof(Common::packet::C2S_RoomEnterAcceptPacket));
		break;
	case PacketType::MovePacket_c2s:
		deserializedPacket = new Common::packet::C2S_MovePacket();
		memcpy(deserializedPacket, _recvBuffer.data(), sizeof(Common::packet::C2S_MovePacket));
		break;
	case PacketType::ErrorPacket:
		__debugbreak();
		break;
	default:
		// 알 수 없는 패킷 타입 처리
		break;
	}
	if (nullptr == deserializedPacket)
	{
		__debugbreak();
	}
	_currentRoom->EnqueuePacket(deserializedPacket);
}

void Session::SendPacket()
{
	Common::packet::PacketHeader* packetToSend = nullptr;
	{
		std::lock_guard<std::mutex> lock(_sendMutex);
		if (_sendQueue.empty())
			return;
		packetToSend = _sendQueue.front();
		_sendQueue.pop();
	}
	int total_sent_len = 0;
	int retval = 0;
	while (total_sent_len < packetToSend->size) {
		retval = send(_socket, reinterpret_cast<char*>(packetToSend) + total_sent_len,
			static_cast<int>(packetToSend->size) - total_sent_len, 0);
		if (SOCKET_ERROR == retval) {
			err_display("send()");
			break;
		}
		else if (0 == retval)
			break;
		total_sent_len += retval;
	}
	delete packetToSend;
}

void Session::RecvPacket()
{
	// 클라에서 준 패킷 데이터를 _recvBuffer에 저장
	int recv_bytes = recv(_socket, _recvBuffer.data(), static_cast<int>(sizeof(Common::packet::PacketHeader)),
		MSG_WAITALL);
	
	Common::packet::PacketHeader* header = reinterpret_cast<Common::packet::PacketHeader*>(_recvBuffer.data());

	unsigned long long total_recv_len = 0;
	int retval = 0;
	while (total_recv_len < header->size) {
		retval = recv(_socket, _recvBuffer.data() + recv_bytes, static_cast<int>(_recvBuffer.size()), 0);
		if (SOCKET_ERROR == retval) {
			err_display("recv()");
			break;
		}
		else if (0 == retval)
			break;
		total_recv_len += retval;
	}
	
}

void Session::Disconnect()
{
	closesocket(_socket);
	_state = ClientState::Disconnected;
}

void Session::EnqueuePacket(Common::packet::PacketHeader* packet)
{
	std::lock_guard<std::mutex> lock(_sendMutex);
	_sendQueue.push(packet);
}
