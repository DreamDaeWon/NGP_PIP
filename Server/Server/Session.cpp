#include "pch.h"
#include "Session.h"
#include "Room.h"



Session::Session() : _id(0), _socket(INVALID_SOCKET), _state(ClientState::Disconnected), _currentRoom{ nullptr }
{
}

Session::Session(uint32_t id, SOCKET socket, ClientState state) : _id(id), _socket(socket), _state(state), _currentRoom{ nullptr }
{
}



Session::~Session()
{
}

void Session::init(uint32_t id, SOCKET socket, ClientState state)
{
	_id = id;
	_socket = socket;
	_state = state;
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
	using namespace common::packet;
	common::packet::PacketHeader* header = reinterpret_cast<common::packet::PacketHeader*>(_recvBuffer.data());
	PacketHeader* deserializedPacket = nullptr;
	switch (header->type)
	{
		// DW수정 : 서버가 받을 패킷은 MovePacket_c2s이거 하나라고 박대원은 인식하고 있음
		// KJ: 로그인 로그아웃은 있어야함
		
		case PacketType::LoginRequestPacket_c2s:
			deserializedPacket = new common::packet::C2S_LoginRequestPacket();
			memcpy(deserializedPacket, _recvBuffer.data(), sizeof(common::packet::C2S_LoginRequestPacket));
			break;
		case PacketType::LogoutPacket_c2s:
			deserializedPacket = new common::packet::C2S_LogoutPacket;
			memcpy(deserializedPacket, _recvBuffer.data(), sizeof(common::packet::C2S_LogoutPacket));
			break;
		case PacketType::MovePacket_c2s:
			deserializedPacket = new common::packet::C2S_MovePacket();
			memcpy(deserializedPacket, _recvBuffer.data(), sizeof(common::packet::C2S_MovePacket));
			break;
		case PacketType::ErrorPacket:
			__debugbreak();
			break;
		default:
			__debugbreak();
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
	std::shared_ptr<std::vector<char>> packetToSend = nullptr;
	{
		std::lock_guard<std::mutex> lock(_sendMutex);
		if (_sendQueue.empty())
			return;
		packetToSend = _sendQueue.front();
		_sendQueue.pop();
	}

	if (packetToSend == nullptr) return;

	int total_sent_len = 0;
	int retval = 0;
	while (total_sent_len < packetToSend->size()) {
		retval = send(_socket, packetToSend->data() + total_sent_len,
			static_cast<int>(packetToSend->size()) - total_sent_len, 0);
		if (SOCKET_ERROR == retval) {
			err_display("send()");
			break;
		}
		else if (0 == retval)
			break;
		total_sent_len += retval;
	}
}

void Session::RecvPacket()
{
	// 클라에서 준 패킷 데이터를 _recvBuffer에 저장
	int retval = recv(_socket, _recvBuffer.data(), static_cast<int>(sizeof(common::packet::PacketHeader)),
		MSG_WAITALL);
	if (SOCKET_ERROR == retval)
	{
		err_display("recv() header error");
		Disconnect();
		return;
	}
	if (0 == retval)
	{
		printf("Client disconnected gracefully.\n");
		Disconnect();
		return;
	}
	
	common::packet::PacketHeader* header = reinterpret_cast<common::packet::PacketHeader*>(_recvBuffer.data());

	int required_bytes = header->size - sizeof(common::packet::PacketHeader);
	if (required_bytes > 0)
	{
	    retval = recv(_socket, _recvBuffer.data() + sizeof(common::packet::PacketHeader), required_bytes,
	MSG_WAITALL);
	    if (retval == SOCKET_ERROR)
	    {
            err_display("recv() body error");
            Disconnect();
            return;
	    }
	    if (retval == 0)
	    {
            printf("Client [%2d] disconnected gracefully.\n", _id);
            Disconnect();
            return;
	    }
	}
	
}

void Session::Disconnect()
{
	// Room에서 플레이어 제거 요청
	if (_currentRoom != nullptr)
	{
		_currentRoom->RemovePlayer(_id);
	}

	closesocket(_socket);
	_state = ClientState::Disconnected;

	// Clear the queue, smart pointers will be released
	std::lock_guard<std::mutex> lock(_sendMutex);
	std::queue<std::shared_ptr<std::vector<char>>> emptyQueue;
	_sendQueue.swap(emptyQueue);
}

void Session::EnqueuePacket(std::shared_ptr<std::vector<char>> packet)
{
	std::lock_guard<std::mutex> lock(_sendMutex);
	_sendQueue.push(packet);
}
