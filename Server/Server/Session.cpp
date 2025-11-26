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
		// 데이터 수신 및 처리
		DoRecv();

		// 데이터 송신
		SendPacket();

		// CPU 과부하 방지를 위한 짧은 대기
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}

void Session::DoRecv()
{
	// 1. 데이터 수신
	int retval = recv(_socket, _recvBuffer.data() + _recvBufferOffset,
		_recvBuffer.size() - _recvBufferOffset, 0);

	if (retval == SOCKET_ERROR)
	{
		int error = WSAGetLastError();
		if (error == WSAEWOULDBLOCK)
		{
			// 논블로킹 소켓에서 수신할 데이터가 없으면 이 오류가 발생합니다. 정상적인 상황입니다.
			return;
		}
		else
		{
			// 그 외의 오류는 연결 종료로 처리합니다.
			err_display("recv() error");
			Disconnect();
			return;
		}
	}
	else if (retval == 0)
	{
		// 상대방이 연결을 정상적으로 종료했습니다.
		printf("Client disconnected gracefully.\n");
		Disconnect();
		return;
	}

	// 수신한 데이터만큼 오프셋 증가
	_recvBufferOffset += retval;

	// 2. 패킷 처리
	using namespace common::packet;
	while (_recvBufferOffset >= sizeof(PacketHeader))
	{
		PacketHeader* header = reinterpret_cast<PacketHeader*>(_recvBuffer.data());
		size_t packetSize = header->size;

		// 전체 패킷이 모두 수신되었는지 확인
		if (_recvBufferOffset < packetSize)
		{
			// 아직 패킷이 다 오지 않았으므로 다음 수신을 기다립니다.
			break;
		}
		/*std::cout << "session:"<< _id
		<< "packet Number: " << static_cast<int>(header->type) << " packet received, size: " << packetSize << " bytes.\n";*/


		// --- 패킷 하나를 처리하는 로직 (기존 ProcessPacket과 유사) ---
		std::unique_ptr<PacketHeader> packet = nullptr;
		switch (header->type)
		{
		case PacketType::LoginRequestPacket_c2s:
		{
			auto loginPacket = std::make_unique<C2S_LoginRequestPacket>();
			memcpy(loginPacket.get(), _recvBuffer.data(), sizeof(C2S_LoginRequestPacket));
			packet = std::move(loginPacket);
			break;
		}
		case PacketType::LogoutPacket_c2s:
		{
			auto logoutPacket = std::make_unique<C2S_LogoutPacket>();
			memcpy(logoutPacket.get(), _recvBuffer.data(), sizeof(C2S_LogoutPacket));
			packet = std::move(logoutPacket);
			break;
		}
		case PacketType::MovePacket_c2s:
		{
			auto movePacket = std::make_unique<C2S_MovePacket>();
			memcpy(movePacket.get(), _recvBuffer.data(), sizeof(C2S_MovePacket));
			packet = std::move(movePacket);
			break;
		}
		default:
			// 알 수 없는 패킷 타입이면 연결을 끊습니다.
			printf("Unknown packet type received. Disconnecting.\n");
			Disconnect();
			return;
		}

		if (packet)
		{
			_currentRoom->EnqueuePacket(std::move(packet));
		}
		// ---------------------------------------------------------

		// 처리한 패킷만큼 버퍼에서 제거
		size_t remainingData = _recvBufferOffset - packetSize;
		if (remainingData > 0)
		{
			memmove(_recvBuffer.data(), _recvBuffer.data() + packetSize, remainingData);
		}
		_recvBufferOffset = remainingData;
	}
}

void Session::SendPacket()
{
	// 보낼 패킷들을 임시 큐로 옮기고 락을 빠르게 해제합니다.
	std::queue<std::shared_ptr<std::vector<char>>> packetsToSend;
	{
		std::lock_guard<std::mutex> lock(_sendMutex);
		if (_sendQueue.empty())
		{
			return;
		}
		packetsToSend.swap(_sendQueue);
	}

	// 락 없이 임시 큐에 있는 모든 패킷을 보냅니다.
	while (!packetsToSend.empty())
	{
		std::shared_ptr<std::vector<char>> packet = packetsToSend.front();

		if (packet == nullptr) 
		{
			packetsToSend.pop();
			continue;
		}

		int total_sent_len = 0;
		int retval = 0;
		while (total_sent_len < packet->size()) {
			retval = send(_socket, packet->data() + total_sent_len,
				static_cast<int>(packet->size()) - total_sent_len, 0);
			
			if (SOCKET_ERROR == retval) {
				int error = WSAGetLastError();
				if (error == WSAEWOULDBLOCK)
				{
					// 버퍼가 꽉 찼으므로 전송을 중단하고, 보내지 못한 패킷들을 다시 메인 큐에 넣습니다.
					std::lock_guard<std::mutex> lock(_sendMutex);
					// 현재 처리 중이던 큐(packetsToSend)를 기존 큐(_sendQueue)와 바꿉니다.
					// 이렇게 하면 보내지 못한 패킷들이 _sendQueue의 앞쪽으로 갑니다.
					_sendQueue.swap(packetsToSend);
					// 그 사이에 새로 추가된 패킷이 있었다면, 그 뒤에 순서대로 붙여줍니다.
					while (!packetsToSend.empty())
					{
						_sendQueue.push(packetsToSend.front());
						packetsToSend.pop();
					}
					return; // 다음 WorkerLoop에서 다시 시도합니다.
				}

				// WSAEWOULDBLOCK 이외의 오류는 연결 끊김으로 처리합니다.
				err_display("send()");
				Disconnect();
				return;
			}
			else if (0 == retval)
			{
				// 이 경우도 연결 문제로 간주하고 중단합니다.
				Disconnect();
				return;
			}
			total_sent_len += retval;
		}

		// 패킷 하나를 성공적으로 보냈으므로 큐에서 제거합니다.
		packetsToSend.pop();
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
