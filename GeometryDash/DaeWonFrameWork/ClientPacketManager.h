#pragma once
#include "Packet.h"
#include "Singleton.h"
#include <functional>
#include <unordered_map>

// CJ : Packet 처리 엔진

using ClientPacketHandlerFunc = std::function<void(char*)>; // unordered_map에서 다양한 함수를 등록 받기 위해
using namespace common::packet; // 클라는 PacketType, PacketHeader를 쓸테니 네임스페이스 사용


class ClientPacketManager
{
public:
	ClientPacketManager();

	// 서버로부터 받은 원시데이터를 어떤 함수가 처리해야할지 결정하고 호출하는 함수
	void HandlePacket(char* buffer);

private:
	// PacketManager 객체를 생성하는 즉시 모든 패킷을 처리할 준비하는 함수
	void RegisterHandler();
	
	std::unordered_map<PacketType, ClientPacketHandlerFunc> _handlers;
};

