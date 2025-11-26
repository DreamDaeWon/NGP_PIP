#include "ClientPacketManager.h"
#include "ClientPakcetHandler.h"

ClientPacketManager::ClientPacketManager()
{
	RegisterHandler();
}

void ClientPacketManager::GetInstance()
{
}

void ClientPacketManager::DestroyInstance()
{
}

void ClientPacketManager::HandlePacket(char* buffer)
{
	// 버퍼에서 패킷 헤더 추출 및 타입 확인 <- 첫 부분을 PacketHeader라고 간주
	PacketHeader* header = (PacketHeader*)buffer;
	PacketType type = header->type;

	// _handlers에서 해당 타입의 핸들러 함수 찾기
	auto iter = _handlers.find(type);
	if (iter != _handlers.end())
	{
		iter->second(buffer);
	}
	else
	{
		// 오류 <- 클라이언트가 모르는 패킷을 서버가 보낸 경우
		// CJ 질문 : 오류 처리 어떻게 할까? <- 로그 남기기?
	}
}

void ClientPacketManager::RegisterHandler()
{
	using namespace client::handler;

	_handlers[PacketType::MovePacket_s2c] = Handle_MovePacket_s2c;
	_handlers[PacketType::LoginAcceptPacket_s2c] = Handle_LoginAccept_s2c;
	_handlers[PacketType::AllPlayerMovePacket_s2c] = Handle_AllPlayerMovePacket_s2c;
	//_handlers[PacketType::MapLoadPacket_s2c] = Handle_MapLoad_s2c;
	//_handlers[PacketType::RoomEnterAcceptPacket_s2c] = Handle_RoomEnterAccept_s2c;
	_handlers[PacketType::RoomStartPacket_s2c] = Handle_RoomStart_s2c;
	//_handlers[PacketType::MapRoomEndPacket_s2c] = Handle_MapRoomEnd_s2c;
}