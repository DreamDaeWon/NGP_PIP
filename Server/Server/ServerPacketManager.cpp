#include "pch.h"
#include "ServerPacketManager.h"

ServerPacketManager::ServerPacketManager()
{
}

void ServerPacketManager::RegisterHandler(PacketType type, PacketHandlerFunc func)
{
}

void ServerPacketManager::HandlePacket(PacketType type, Session* session)
{
}
