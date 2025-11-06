#pragma once

namespace client::handler
{
	void Handle_MovePacket_s2c(char* buffer);
	void Handle_LoginAccept_s2c(char* buffer);
	//void Handle_MapNames_s2c(char* buffer);
	//void Handle_MapLoad_s2c(char* buffer);
	void Handle_RoomEnterAccept_s2c(char* buffer);
	void Handle_RoomStart_s2c(char* buffer);
	void Handle_MapRoomEnd_s2c(char* buffer);
}