#include "Player.h"
#include "ObjManager.h"
#include "CameraManager.h"
#include "KeyManager.h"
#include "CameraManager.h"
#include "CollisionManager.h"
#include "Tile.h"
#include "CSoundManager.h"
#include "StageManager.h"

//#pragma comment (lib, "msimg32.lib")
CPlayer::CPlayer()
{
    Initailizer();
}

CPlayer::~CPlayer()
{
    Free();
}

void CPlayer::Initailizer()
{
    HBITMAP* InBitMap{};
    LoadPlayerSound();
    TCHAR Name[50] = {};
    wsprintf(Name, L"../resource/Player/player.bmp");
    InBitMap = new HBITMAP;
    *InBitMap = (HBITMAP)LoadImage(g_hInst, Name, IMAGE_BITMAP, 128, 128, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
    m_vechBitMap.push_back(InBitMap);

    wsprintf(Name, L"../resource/Player/Ship.bmp");
    InBitMap = new HBITMAP;
    *InBitMap = (HBITMAP)LoadImage(g_hInst, Name, IMAGE_BITMAP, 128, 128, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
    m_vechBitMap.push_back(InBitMap);

    wsprintf(Name, L"../resource/Player/Zigzag.bmp");
    InBitMap = new HBITMAP;
    *InBitMap = (HBITMAP)LoadImage(g_hInst, Name, IMAGE_BITMAP, 128, 128, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
    m_vechBitMap.push_back(InBitMap);

    wsprintf(Name, L"../resource/Mainmenu/StageComplete.bmp");
    InBitMap = new HBITMAP;
    *InBitMap = (HBITMAP)LoadImage(g_hInst, Name, IMAGE_BITMAP, 800, 600, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
    m_vechBitMap.push_back(InBitMap);

    CSoundManager::GetInstance()->LoadSound("GeometryDashLevelComplete", "../sound/Geometry Dash Level Complete - djlunatique.mp3");

}

int CPlayer::Update(float fTime)
{

    m_fTime += fTime * 10.f;
    KeyDown();
    float ridius{};
    switch (m_NowDir)
    {
    case CPlayer::DIR_LEFT:
        m_CenterPos.x -= m_fSpeed * fTime;
        break;
    case CPlayer::DIR_RIGHT:
        if (m_eStatus != STATUS_FINISH && m_eStatus != STATUS_DIE) {
            m_CenterPos.x += m_fSpeed * fTime;
        }
        break;
    case CPlayer::DIR_UP:
        m_CenterPos.y -= m_fSpeed * fTime;
        break;
        /*case CPlayer::DIR_DOWN:
           m_CenterPos.y += m_fSpeed * fTime;
           break;*/
    }

    switch (m_eStatus)
    {
    case CPlayer::STATUS_NOMAL: {
        MSpace(fTime);
        if (Key[DIR_DOWN]) {
            m_CenterPos.y += m_fJumpSpeed * fTime;
            angle = angle + SpinSpeed;
        }
        break;
    }
    case CPlayer::STATUS_AIRPLANE: {
        if (CKeyManager::GetInstance()->KeyPressing(VK_SPACE)) {
            if (angle > -60.f) {
                angle = angle - 4.f;
            }
        }
        else if (angle < 60.f)
        {
            angle = angle + 4.f;
        }
        else if (angle <= -60.f)
        {
            angle = -60.f;
        }
        else if (angle >= 60.f)
        {
            angle = 60.f;
        }

        m_CenterPos.y += m_fSpeed * fTime * (angle / 45.f);
        break;
    }
    case CPlayer::STATUS_ZIGZAG: {
        if (m_eZigzag == ZIGZAG_UP) {
            m_CenterPos.y -= m_fSpeed * fTime;
        }
        else if (m_eZigzag == ZIGZAG_DOWN) {
            m_CenterPos.y += m_fSpeed * fTime;
        }
        break;
    }
    case CPlayer::STATUS_FINISH: {
        if (m_Fcntx < (90 * fspeed)) {
            m_CenterPos.x += m_fSpeed / fspeed * fTime * (m_Fcntx / (30.f * fspeed));
            angle = angle + (4 / fspeed);
            ++m_Fcntx;
        }
        if (m_Fcnty < (50 * fspeed)) {
            m_CenterPos.y -= m_fSpeed / fspeed * fTime * (m_Fcnty / (90.f * fspeed));
            ++m_Fcnty;
        }
        else if (50 * (fspeed <= m_Fcnty) && m_Fcnty < (90 * fspeed)) {
            m_CenterPos.y += m_fSpeed / fspeed * fTime * (m_Fcnty / (90.f * fspeed));
            ++m_Fcnty;
        }
        break;

    case CPlayer::STATUS_DIE:
    {

        m_fRidius -= fTime * 100.f;

        if (m_fRidius <= 0.f)
        {
            CStageManager::GetInstance()->StageChanger((CStageManager::STAGETYPE)CStageManager::GetInstance()->GetNowSatge());
        }
    }
    break;
    }
    default:
        break;
    }
    ridius = sqrt((m_fRidius * m_fRidius) + ((m_fRidius) * (m_fRidius)));
    m_NowPoint[0].x = (m_CenterPos.x - CCameraManager::GetInstance()->GetCenterPos().x) + (ridius * cos(radian(angle - 135)));
    m_NowPoint[0].y = (m_CenterPos.y - CCameraManager::GetInstance()->GetCenterPos().y) + (ridius * sin(radian(angle - 135)));

    m_NowPoint[1].x = (m_CenterPos.x - CCameraManager::GetInstance()->GetCenterPos().x) + (ridius * cos(radian(angle - 45)));
    m_NowPoint[1].y = (m_CenterPos.y - CCameraManager::GetInstance()->GetCenterPos().y) + (ridius * sin(radian(angle - 45)));

    m_NowPoint[2].x = (m_CenterPos.x - CCameraManager::GetInstance()->GetCenterPos().x) + (ridius * cos(radian(angle - 225)));
    m_NowPoint[2].y = (m_CenterPos.y - CCameraManager::GetInstance()->GetCenterPos().y) + (ridius * sin(radian(angle - 225)));
    return 0;
}

void CPlayer::LateUpdate(float fTime)
{
    if ((int)m_fTime >= m_vechBitMap.size())
    {
        m_fTime = 0.f;
    }
    if (m_eStatus == STATUS_NOMAL) {
        m_eZigzag = ZIGZAG_DOWN;
        if (m_Jcnt >= m_JumpHeight) {
            Key[DIR_DOWN] = true;
        }
    }
    else if (m_eStatus == STATUS_AIRPLANE) {
        m_eZigzag = ZIGZAG_DOWN;
        if (Key[DIR_SPACE] == false) {
            Key[DIR_DOWN] = true;
        }
    }
    else if (m_eStatus == STATUS_ZIGZAG) {


    }
    Collision();

}

void CPlayer::Render(HDC mDC)
{
    HDC _BackDc{};
    HBITMAP _BackBitmap{};

    _BackDc = CreateCompatibleDC(mDC);
    _BackBitmap = CreateCompatibleBitmap(mDC, WINSIZEX, WINSIZEY);
    SelectObject(_BackDc, _BackBitmap);


    m_PalyerDc = CreateCompatibleDC(_BackDc);

    if (m_eBeforeStatus == STATUS_END)
    {
        switch (m_eStatus)
        {
        case CPlayer::STATUS_NOMAL:
            SelectObject(m_PalyerDc, (BITMAP*)*(m_vechBitMap[STATUS_NOMAL]));
            break;
        case CPlayer::STATUS_AIRPLANE:
            SelectObject(m_PalyerDc, (BITMAP*)*(m_vechBitMap[STATUS_AIRPLANE]));
            break;
        case CPlayer::STATUS_ZIGZAG:
            SelectObject(m_PalyerDc, (BITMAP*)*(m_vechBitMap[STATUS_ZIGZAG]));
            break;
        }
    }
    else
    {
        switch (m_eBeforeStatus)
        {
        case CPlayer::STATUS_NOMAL:
            SelectObject(m_PalyerDc, (BITMAP*)*(m_vechBitMap[STATUS_NOMAL]));
            break;
        case CPlayer::STATUS_AIRPLANE:
            SelectObject(m_PalyerDc, (BITMAP*)*(m_vechBitMap[STATUS_AIRPLANE]));
            break;
        case CPlayer::STATUS_ZIGZAG:
            SelectObject(m_PalyerDc, (BITMAP*)*(m_vechBitMap[STATUS_ZIGZAG]));
            break;
        }
    }


    // 테두리 지우는 코드 넣기
    HPEN hPen = (HPEN)CreatePen(PS_SOLID, 2, RGB(255, 0, 255)), OldPen{};
    OldPen = (HPEN)SelectObject(_BackDc, hPen);

    HBRUSH hBrush = CreateSolidBrush(RGB(255, 0, 255)), OldBrush{};
    OldBrush = (HBRUSH)SelectObject(_BackDc, hBrush);

    Rectangle(_BackDc, 0, 0, WINSIZEX, WINSIZEY);

    SelectObject(_BackDc, OldBrush);
    SelectObject(_BackDc, OldPen);
    DeleteObject(hBrush);
    DeleteObject(hPen);
    // 여기서부터 그리기 하기!
    if (m_Fcntx >= (90 * fspeed)) {
        m_CompleteDc = CreateCompatibleDC(_BackDc);
        SelectObject(m_CompleteDc, (BITMAP*)*(m_vechBitMap[3]));
        TransparentBlt(_BackDc, 0, 0, 800, 600, m_CompleteDc, 0, 0, 800, 600, RGB(132, 126, 135));
    }

    PlgBlt(_BackDc, m_NowPoint, m_PalyerDc, 0, 0, 128, 128, NULL, 0, 0);

    TransparentBlt(mDC, 0, 0, WINSIZEX, WINSIZEY, _BackDc, 0, 0, WINSIZEX, WINSIZEY, RGB(255, 0, 255));

    DeleteObject(m_CompleteDc);
    DeleteObject(m_PalyerDc);
    DeleteDC(_BackDc);
    DeleteObject(_BackBitmap);

}



void CPlayer::Free()
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

void CPlayer::KeyDown()
{
    // KeyManager 사용예시
    if (CKeyManager::GetInstance()->KeyDown(VK_LEFT))
    {
        Key[DIR_LEFT] = true;
        m_NowDir = DIR_LEFT;
    }
    else if (CKeyManager::GetInstance()->KeyUp(VK_LEFT))
    {
        Key[DIR_LEFT] = false;
    }

    if (CKeyManager::GetInstance()->KeyDown(VK_RIGHT))
    {
        Key[DIR_RIGHT] = true;
        m_NowDir = DIR_RIGHT;
    }
    else if (CKeyManager::GetInstance()->KeyUp(VK_RIGHT))
    {
        Key[DIR_UP] = false;
    }
    if (CKeyManager::GetInstance()->KeyDown(VK_UP))
    {
        Key[DIR_UP] = true;
        m_NowDir = DIR_UP;
    }
    else if (CKeyManager::GetInstance()->KeyUp(VK_UP))
    {
        Key[DIR_UP] = false;
    }

    if (CKeyManager::GetInstance()->KeyDown(VK_DOWN))
    {
        Key[DIR_DOWN] = true;
        m_NowDir = DIR_DOWN;
    }
    else if (CKeyManager::GetInstance()->KeyUp(VK_DOWN))
    {
        Key[DIR_DOWN] = false;
    }

    if (CKeyManager::GetInstance()->KeyDown(VK_SPACE))
    {
        if (m_eStatus == STATUS_NOMAL) {
            if (!m_bMSpace && Key[DIR_SPACE] == false) {
                m_Jcnt = 0;
            }
            Key[DIR_DOWN] = false;
            Key[DIR_SPACE] = true;
            m_bMSpace = true;
        }
        else if (m_eStatus == STATUS_AIRPLANE) {
            Key[DIR_SPACE] = true;
            Key[DIR_DOWN] = false;

        }
        else if (m_eStatus == STATUS_ZIGZAG) {
            Key[DIR_SPACE] = true;
            Key[DIR_DOWN] = false;
            angle = -45;
            m_eZigzag = ZIGZAG_UP;
        }

    }
    else if (CKeyManager::GetInstance()->KeyUp(VK_SPACE))
    {

        if (m_eStatus == STATUS_NOMAL) {
            Key[DIR_SPACE] = false;
        }
        else if (m_eStatus == STATUS_AIRPLANE) {
            Key[DIR_SPACE] = false;
        }
        else if (m_eStatus == STATUS_ZIGZAG) {

            Key[DIR_SPACE] = false;
            angle = 45;
            m_eZigzag = ZIGZAG_DOWN;
        }
    }
}

void CPlayer::Collision()
{
    m_POSRECT = RECT{ (long)(m_CenterPos.x - m_fRidius), (long)(m_CenterPos.y - m_fRidius), (long)(m_CenterPos.x + m_fRidius), (long)(m_CenterPos.y + m_fRidius) };

    if (m_POSRECT.left < 0)
    {
        m_NowDir = DIR_RIGHT;
    }

    //if (m_POSRECT.right > CCameraManager::GetInstance()->GetBackImageSize().x)
 //   {
     //   m_NowDir = DIR_LEFT;
   // }

    if (m_POSRECT.top < 0)
    {
        m_eBeforeStatus = m_eStatus;
        m_eStatus = STATUS_DIE;

    }

    if (m_POSRECT.bottom >= CCameraManager::GetInstance()->GetBackImageSize().y)
    {
        m_eBeforeStatus = m_eStatus;
        m_eStatus = STATUS_DIE;
  
    }





    for (auto iter = CObjManager::GetInstance()->GetAllVector()[CObjManager::OBJECT_TILE].begin();
        iter != CObjManager::GetInstance()->GetAllVector()[CObjManager::OBJECT_TILE].end();
        ++iter)
    {


        if (dynamic_cast<CTile*>(*iter)->GetTileType() == CTile::TILE_STONE)
        {
            if (CCollisionManager::GetInstance()->CheckRect(this, (*iter)) && m_eStatus != STATUS_DIE)
            {
                m_POSRECT = RECT{ (long)(m_CenterPos.x - m_fRidius), (long)(m_CenterPos.y - m_fRidius), (long)(m_CenterPos.x + m_fRidius), (long)(m_CenterPos.y + m_fRidius) };
                if (m_eStatus == STATUS_NOMAL) {
                    if (m_CenterPos.y <= (*iter)->GetPosRect().bottom && m_CenterPos.y >= (*iter)->GetPosRect().top) {
                        m_eBeforeStatus = m_eStatus;
                        m_eStatus = STATUS_DIE;
                    }
                    else {
                        if (m_CenterPos.y < (((*iter)->GetPosRect().bottom + (*iter)->GetPosRect().top) / 2)) {
                            m_CenterPos.y = (*iter)->GetPosRect().top - m_fRidius;
                            m_Jcnt = m_JumpHeight;
                            angle = 0.f;
                            m_NowPoint[0] = { (long)(m_CenterPos.x - CCameraManager::GetInstance()->GetCenterPos().x - m_fRidius),(long)((m_CenterPos.y - CCameraManager::GetInstance()->GetCenterPos().y - m_fRidius)) };

                            m_NowPoint[1] = { (long)(m_CenterPos.x - CCameraManager::GetInstance()->GetCenterPos().x + m_fRidius),(long)(m_CenterPos.y - CCameraManager::GetInstance()->GetCenterPos().y - m_fRidius) };

                            m_NowPoint[2] = { (long)(m_CenterPos.x - CCameraManager::GetInstance()->GetCenterPos().x - m_fRidius),(long)(m_CenterPos.y - CCameraManager::GetInstance()->GetCenterPos().y + m_fRidius) };

                            m_bMSpace = false;

                            m_JumpHeight = 15;
                        }
                        else {
                            m_eBeforeStatus = m_eStatus;
                            m_eStatus = STATUS_DIE;
                        }

                    }
                }
                else if (m_eStatus == STATUS_AIRPLANE) {
                    if (m_CenterPos.y <= (*iter)->GetPosRect().bottom && m_CenterPos.y >= (*iter)->GetPosRect().top) {
                        m_eBeforeStatus = m_eStatus;
                        m_eStatus = STATUS_DIE;
                    }
                    else {
                        if (m_CenterPos.y < (((*iter)->GetPosRect().bottom + (*iter)->GetPosRect().top) / 2)) {
                            m_CenterPos.y = (*iter)->GetPosRect().top - m_fRidius;
                            angle = -2;
                        }
                        else {
                            m_CenterPos.y = (*iter)->GetPosRect().bottom + m_fRidius;
                            angle = 2;
                        }
                    }
                }
                else if (m_eStatus == STATUS_ZIGZAG) {
                    m_eBeforeStatus = m_eStatus;
                    m_eStatus = STATUS_DIE;
                }

            }
        }

    }

    if ((CCameraManager::GetInstance()->GetBackImageSize().x - m_POSRECT.right) < (450) && m_eStatus != STATUS_FINISH)
    {
        CSoundManager::GetInstance()->PlayEffectSound("GeometryDashLevelComplete", CSoundManager::CHANNEL_PLAYER_SOUND);
        CSoundManager::GetInstance()->SetVolume(CSoundManager::CHANNEL_PLAYER_SOUND, 0.5f);
        m_eBeforeStatus = m_eStatus;
        m_eStatus = STATUS_FINISH;
    }

}

void CPlayer::MSpace(float fTime)
{
    if (m_eStatus == STATUS_NOMAL) {
        if (m_bMSpace) {
            if (m_Jcnt < m_JumpHeight) {
                m_CenterPos.y -= m_fJumpSpeed * fTime;
                ++m_Jcnt;
                angle = angle + SpinSpeed;
            }
        }
    }
}

float CPlayer::radian(float degrees)
{
    return degrees * (PI / 180);
}

void CPlayer::LoadPlayerSound()
{
    //CSoundManager::GetInstance()->LoadSound("레벨완료","../sound/Geometry Dash Level Complete - djlunatique.com.mp3");


}

//void CPlayer::SetStopSpin()
//{
//   m_Jcnt = JumpCnt;
//   angle = -SpinSpeed;
//   m_bMSpace = false;
//}
