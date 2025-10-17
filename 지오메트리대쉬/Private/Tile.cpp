#include "Tile.h"
#include "CameraManager.h"
#include "ObjManager.h"
#include "Object.h"
#include "CollisionManager.h"
#include "Player.h"
#include "KeyManager.h"

CTile::CTile()
{
    Initailizer();
}

CTile::~CTile()
{
    Free();
}

void CTile::Initailizer()
{

    HBITMAP* InBitMap{};
    TCHAR Name[50] = {};
    for (int i = 1; i <= AllTileNum; ++i)
    {
        wsprintf(Name, L"../resource/Object/사용 타일/타일%d.bmp", i);
        InBitMap = new HBITMAP;
        *InBitMap = (HBITMAP)LoadImage(g_hInst, Name, IMAGE_BITMAP, 128, 128, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
        m_vechBitMap.push_back(InBitMap);
    }
    m_fRidius = TileSizeX / 2.f;


}

int CTile::Update(float fTime)
{
    if (m_eTileType >= TILE_POTAL_NOMAL_FRONT && m_eTileType <= TILE_POTAL_LEFT_RIGHT_CHANGE_BACK)
    {
        m_fRidius = TileSizeX;
    }

    m_POSRECT.left = m_CenterPos.x - m_fRidius;
    m_POSRECT.right = m_CenterPos.x + m_fRidius;
    m_POSRECT.top = m_CenterPos.y - m_fRidius;
    m_POSRECT.bottom = m_CenterPos.y + m_fRidius;
    return 0;
}

void CTile::LateUpdate(float fTime)
{
    Collision();
}

void CTile::Render(HDC mDC)
{
    m_TileDc = CreateCompatibleDC(mDC);

    SelectObject(m_TileDc, *(m_vechBitMap[m_eTileType]));

    TransparentBlt(mDC, m_CenterPos.x - m_fRidius - CCameraManager::GetInstance()->GetCenterPos().x,
        m_CenterPos.y - m_fRidius - CCameraManager::GetInstance()->GetCenterPos().y,
        m_fRidius * 2.f, m_fRidius * 2.f, m_TileDc, 0, 0, 128, 128, RGB(255, 0, 255));

    DeleteObject(m_TileDc);

}

void CTile::Free()
{
    if (m_vechBitMap.size())
    {
        for (auto iter = m_vechBitMap.begin(); iter != m_vechBitMap.end(); ++iter)
        {
            DeleteObject(*(*iter));

            delete (*iter);
            (*iter) = nullptr;
        }
        m_vechBitMap.clear();
    }
}

void CTile::Collision()
{

    if (CObjManager::GetInstance()->GetAllVector()[CObjManager::OBJECT_PLAYER].size() == 0)
    {
        return;
    }
    CObject* pPlayer = CObjManager::GetInstance()->GetAllVector()[CObjManager::OBJECT_PLAYER].front();



    if (m_eTileType >= TILE_SPLINTER_UP && m_eTileType <= TILE_SPLINTER_DOWN)
    {
        if (CCollisionManager::GetInstance()->CheckCircle(this, pPlayer) && dynamic_cast<CPlayer*>(pPlayer)->Get_Status() != CPlayer::STATUS_DIE)
        {
            dynamic_cast<CPlayer*>(pPlayer)->Set_BeforeStatus(dynamic_cast<CPlayer*>(pPlayer)->Get_Status());
            dynamic_cast<CPlayer*>(pPlayer)->Set_Status(CPlayer::STATUS_DIE);
        }
    }
    //else if (m_eTileType == TILE_STONE)
    //{
    //   if (CCollisionManager::GetInstance()->CheckCircle(this, pPlayer))
    //   {
    //      pPlayer->SetCenterPos(POINT{ pPlayer->GetCenterPos().x, (LONG)(m_POSRECT.top - pPlayer->GetRidius())});
    //      dynamic_cast<CPlayer*>(pPlayer)->SetStopSpin();
    //   }
    //}
    else if (m_eTileType == TILE_POTAL_NOMAL_LEFT_RIGHT_FRONT)
    {
        if (CCollisionManager::GetInstance()->CheckCircle(this, pPlayer))
        {
            CCameraManager::GetInstance()->SetNomalLeftRight();
        }
    }

    else if (m_eTileType == TILE_POTAL_LEFT_RIGHT_CHANGE_FRONT)
    {
        if (CCollisionManager::GetInstance()->CheckCircle(this, pPlayer))
        {
            CCameraManager::GetInstance()->SetChangeLeftRight();
        }
    }
    else if (m_eTileType == TILE_POTAL_AIR_PLANE_FRONT)
    {
        if (CCollisionManager::GetInstance()->CheckCircle(this, pPlayer))
        {
            if (dynamic_cast<CPlayer*>(pPlayer)->Get_Status() != CPlayer::STATUS_AIRPLANE)
            {
                dynamic_cast<CPlayer*>(pPlayer)->SetAngle(0.f);
            }
            dynamic_cast<CPlayer*>(pPlayer)->Set_Status(CPlayer::STATUS_AIRPLANE);
        }
    }
    else if (m_eTileType == TILE_POTAL_ZIGZAG_FRONT)
    {
        if (CCollisionManager::GetInstance()->CheckCircle(this, pPlayer))
        {
            if (dynamic_cast<CPlayer*>(pPlayer)->Get_Status() != CPlayer::STATUS_ZIGZAG)
            {
                dynamic_cast<CPlayer*>(pPlayer)->SetAngle(45.f);
            }
            dynamic_cast<CPlayer*>(pPlayer)->Set_Status(CPlayer::STATUS_ZIGZAG);
        }
    }
    else if (m_eTileType == TILE_POTAL_NOMAL_FRONT)
    {
        if (CCollisionManager::GetInstance()->CheckCircle(this, pPlayer))
        {
            if (dynamic_cast<CPlayer*>(pPlayer)->Get_Status() != CPlayer::STATUS_NOMAL)
            {
                dynamic_cast<CPlayer*>(pPlayer)->SetAngle(0.f);
            }
            dynamic_cast<CPlayer*>(pPlayer)->Set_Status(CPlayer::STATUS_NOMAL);
        }
    }
    else if (m_eTileType == TILE_JUMP_CIRCLE)
    {
        if (CCollisionManager::GetInstance()->CheckCircle(this, pPlayer) &&
            CKeyManager::GetInstance()->KeyDown(VK_SPACE))
        {
            dynamic_cast<CPlayer*>(pPlayer)->SetJump();
        }
    }
    else if (m_eTileType == TILE_JUMP_FLOOR)
    {
        if (CCollisionManager::GetInstance()->CheckCircle(this, pPlayer))
        {
            if (dynamic_cast<CPlayer*>(pPlayer)->Get_Status() == CPlayer::STATUS_NOMAL)
            {
                dynamic_cast<CPlayer*>(pPlayer)->SetJumpHeight();
                dynamic_cast<CPlayer*>(pPlayer)->SetJump();
            }
            else if (dynamic_cast<CPlayer*>(pPlayer)->Get_Status() != CPlayer::STATUS_DIE) {
                dynamic_cast<CPlayer*>(pPlayer)->Set_BeforeStatus(dynamic_cast<CPlayer*>(pPlayer)->Get_Status());
                dynamic_cast<CPlayer*>(pPlayer)->Set_Status(CPlayer::STATUS_DIE);
            }
        }
    }
}
