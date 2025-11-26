#include "ClientPakcetHandler.h"

#include <string>

#include "NetworkManager.h"
#include "Packet.h"
#include "ObjManager.h"
#include "OtherPlayer.h"
using namespace common::packet;
void client::handler::Handle_AllPlayerMovePacket_s2c(char* buffer)
{
    S2C_AllPlayerMovePacket* packet = reinterpret_cast<S2C_AllPlayerMovePacket*>(buffer);
    auto players = CObjManager::GetInstance()->GetAllVector()[CObjManager::OBJECT_OTHERPLAYER];
    for (size_t i = 0; i < 3; ++i)
    {
        int targetID = i;
        // 내 캐릭터의 움직임은 서버에서 받아도 무시 (클라 주도)
        if (targetID == CObjManager::GetInstance()->GetMyPlayerID())
            continue;
        // 다른 플레이어 찾기
        COtherPlayer* pTarget = CObjManager::GetInstance()->FindOtherPlayer(targetID);
        if (pTarget != nullptr)
        {
            // 찾았다! 위치 동기화
			POINT pos = { static_cast<LONG>(packet->x[i]), static_cast<LONG>(packet->y[i]) };
            pTarget->SetCenterPos(pos);
            pTarget->SetAngle(packet->Rotate[i]);
            pTarget->SetRidius(packet->ridius[i]);
            pTarget->Set_Status(static_cast<int>(packet->state[i]));
        }
	}
}


void client::handler::Handle_MovePacket_s2c(char* buffer)
{
    // 위치 나중에 추가 할거라 일단 주석
    //PacketHeader* header = (PacketHeader*)buffer;
    //auto packet = reinterpret_cast<S2C_MovePacket*>(buffer); // MovePacket_s2c 구조체 이름 확인 필요

    //int targetID = packet->playerID;

    //// 내 캐릭터의 움직임은 서버에서 받아도 무시 (클라 주도)
    //if (targetID == CObjManager::GetInstance()->GetMyPlayerID())
    //    return;

    //// 다른 플레이어 찾기
    //COtherPlayer* pTarget = CObjManager::GetInstance()->FindOtherPlayer(targetID);

    //if (pTarget != nullptr)
    //{
    //    // 찾았다! 위치 동기화
    //    pTarget->SetPos(packet->x, packet->y);
    //    pTarget->SetAngle(packet->rotate);
    //    // 상태 동기화 등 추가 가능
    //}
    //else
    //{
    //  
    //    // 생성하고 위치 설정
    //    CObjManager::GetInstance()->AddOtherPlayer(targetID);

    //    // 방금 생성된 애 다시 찾아서 위치 설정
    //    pTarget = CObjManager::GetInstance()->FindOtherPlayer(targetID);
    //    if (pTarget)
    //    {
    //        pTarget->SetPos(packet->x, packet->y);
    //        pTarget->SetAngle(packet->rotate);
    //    }
    //}
}

void client::handler::Handle_LoginAccept_s2c(char* buffer)
{
    PacketHeader* header = (PacketHeader*)buffer;

    if (header->type != PacketType::LoginAcceptPacket_s2c) return;

    auto packet = reinterpret_cast<S2C_LoginAcceptPacket*>(buffer);
    int assignedID = packet->playerID;

    // 내 ID 설정
    CObjManager::GetInstance()->SetMyPlayerID(assignedID);
	string debugMsg = "[Client] Login Success! My ID: " + to_string(assignedID) + "\n";
    OutputDebugStringA(debugMsg.c_str());
}

void client::handler::Handle_RoomWait_s2c(char* buffer)
{
    std::printf("[Client] Waiting for players...\n");
}

void client::handler::Handle_RoomStart_s2c(char* buffer)
{
    std::printf("[Client] Room Start!\n");
}
void client::handler::Handle_MapRoomEnd_s2c(char* buffer)
{
}


