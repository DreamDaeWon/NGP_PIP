#pragma once

namespace client::handler
{
	void Handle_MovePacket_s2c(char* buffer);
	void AllPlayer_MovePacket_s2c(char* buffer);
	void Handle_LoginAccept_s2c(char* buffer);
	void Handle_RoomWait_s2c(char* buffer);
	void Handle_RoomStart_s2c(char* buffer);
	void Handle_MapRoomEnd_s2c(char* buffer);

	void SpawnOtherPlayerPacket_s2c(char* buffer);
	void DespawnOtherPlayerPacket_s2c(char* buffer);
	void Handle_AllPlayerMovePacket_s2c(char* buffer);
}