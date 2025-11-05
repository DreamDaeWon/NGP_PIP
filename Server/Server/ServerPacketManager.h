#pragma once
#include "Packet.h"
#include <functional>
#include <unordered_map>

// CJ : Packet 처리 엔진

class Session; // Session 만들어지면 사용하기 위해 전방 선언

using PacketHandlerFunc = std::function<void(Session*, char*)>; // unordered_map에서 다양한 함수를 등록 받기 위해
using Common::packet::PacketType;

// 임시 싱글톤 템플릿
template <typename T>
class Singleton
{
protected:
	Singleton() = default;
	virtual ~Singleton() = default;

public:
	Singleton(const Singleton&) = delete;
	Singleton& operator=(const Singleton&) = delete;

	static T* Instance()
	{
		static T instance;
		return &instance;
	}
};

class ServerPacketManager : public Singleton<ServerPacketManager>
{
public:
	ServerPacketManager();
	
	void RegisterHandler(PacketType type, PacketHandlerFunc func);
	void HandlePacket(PacketType type, Session* session);

private:
	std::unordered_map<PacketType, PacketHandlerFunc> Handlers;

};

