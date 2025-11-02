#pragma once
#include "Packet.h"
#include <functional>
#include <unordered_map>

// CJ : Packet 처리 엔진

class Session; // Session 만들어지면 사용하기 위해 전방 선언

using PacketHandlerFunc = std::function<void(Session*, char*)>; // unordered_map에서 다양한 함수를 등록 받기 위해
using namespace Common::packet;


class ServerPacketManager
{
public:
	ServerPacketManager();
	
	void RegisterHandler(PacketType type, PacketHandlerFunc func);
	void HandlePacket(PacketType type, Session* session);

private:
	std::unordered_map<PacketType, PacketHandlerFunc> Handlers;

};

