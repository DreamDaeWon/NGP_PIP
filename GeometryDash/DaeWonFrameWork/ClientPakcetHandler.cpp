#include "ClientPakcetHandler.h"
#include "Packet.h"
#include "ObjManager.h"
using namespace common::packet;

void client::handler::Handle_MovePacket_s2c(char* buffer)
{
}

void client::handler::Handle_LoginAccept_s2c(char* buffer)
{
	
	auto header = reinterpret_cast<PacketHeader*>(buffer);
	if (header->type != PacketType::LoginAcceptPacket_s2c)
		return;

	auto packet = reinterpret_cast<S2C_LoginRequestPacket*>(buffer);
	int assignedID = packet->playerID;

	CObjManager::GetInstance()->SetPlayerID(assignedID);
	std::printf("[Client] LoginAccept: assigned playerID=%d\n", assignedID);
}

void client::handler::Handle_RoomWait_s2c(char* buffer)
{
}

void client::handler::Handle_RoomStart_s2c(char* buffer)
{
}

void client::handler::Handle_MapRoomEnd_s2c(char* buffer)
{
}
