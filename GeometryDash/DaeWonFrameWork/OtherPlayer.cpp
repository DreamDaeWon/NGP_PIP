#include "OtherPlayer.h"
#include "ObjManager.h"
#include "CameraManager.h"
#include "KeyManager.h"
#include "CollisionManager.h"
#include "Tile.h"
#include "CSoundManager.h"
#include "StageManager.h"

//#pragma comment (lib, "msimg32.lib")
COtherPlayer::COtherPlayer()
{
    Initailizer();
}

COtherPlayer::~COtherPlayer()
{
    Free();
}

void COtherPlayer::Initailizer()
{
    HBITMAP* InBitMap{};
    LoadPlayerSound();
    TCHAR Name[50] = {};
    wsprintf(Name, L"../resource/Player/OtherPlayer1.bmp");
    InBitMap = new HBITMAP;
    *InBitMap = (HBITMAP)LoadImage(g_hInst, Name, IMAGE_BITMAP, 128, 128, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
    m_vechBitMap.push_back(InBitMap);

    wsprintf(Name, L"../resource/Player/OtherPlayer1_Ship.bmp");
    InBitMap = new HBITMAP;
    *InBitMap = (HBITMAP)LoadImage(g_hInst, Name, IMAGE_BITMAP, 128, 128, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
    m_vechBitMap.push_back(InBitMap);

    wsprintf(Name, L"../resource/Player/OtherPlayer1_ZigZag.bmp");
    InBitMap = new HBITMAP;
    *InBitMap = (HBITMAP)LoadImage(g_hInst, Name, IMAGE_BITMAP, 128, 128, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
    m_vechBitMap.push_back(InBitMap);

    wsprintf(Name, L"../resource/Mainmenu/StageComplete.bmp");
    InBitMap = new HBITMAP;
    *InBitMap = (HBITMAP)LoadImage(g_hInst, Name, IMAGE_BITMAP, 800, 600, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
    m_vechBitMap.push_back(InBitMap);
}

int COtherPlayer::Update(float fTime)
{
    float ridius{};
    // 부드러운 보간으로 목표 위치로 이동
    float lerpFactor = LERP_SPEED;

    m_CenterPos.x += (_targetPos.x - m_CenterPos.x) * lerpFactor;
    m_CenterPos.y += (_targetPos.y - m_CenterPos.y) * lerpFactor;

    // 각도 보간
    float angleDiff = _targetAngle - angle;
    if (angleDiff > 180) angleDiff -= 360;
    else if (angleDiff < -180) angleDiff += 360;
    angle += angleDiff * lerpFactor;

    m_fTime += fTime * 10.f;

    ridius = sqrt((m_fRidius * m_fRidius) + ((m_fRidius) * (m_fRidius)));
    m_NowPoint[0].x = (m_CenterPos.x - CCameraManager::GetInstance()->GetCenterPos().x) + (ridius * cos(radian(angle -
        135)));
    m_NowPoint[0].y = (m_CenterPos.y - CCameraManager::GetInstance()->GetCenterPos().y) + (ridius * sin(radian(angle -
        135)));

    m_NowPoint[1].x = (m_CenterPos.x - CCameraManager::GetInstance()->GetCenterPos().x) + (ridius * cos(radian(angle -
        45)));
    m_NowPoint[1].y = (m_CenterPos.y - CCameraManager::GetInstance()->GetCenterPos().y) + (ridius * sin(radian(angle -
        45)));

    m_NowPoint[2].x = (m_CenterPos.x - CCameraManager::GetInstance()->GetCenterPos().x) + (ridius * cos(radian(angle -
        225)));
    m_NowPoint[2].y = (m_CenterPos.y - CCameraManager::GetInstance()->GetCenterPos().y) + (ridius * sin(radian(angle -
        225)));

    return 0;
}

void COtherPlayer::LateUpdate(float fTime)
{

}

void COtherPlayer::Render(HDC mDC)
{
    HDC _BackDc{};
    HBITMAP _BackBitmap{};

    _BackDc = CreateCompatibleDC(mDC);
    _BackBitmap = CreateCompatibleBitmap(mDC, WINSIZEX, WINSIZEY);
    SelectObject(_BackDc, _BackBitmap);


    m_PalyerDc = CreateCompatibleDC(_BackDc);


    switch (m_eStatus)
    {
    case COtherPlayer::STATUS_NOMAL:
        SelectObject(m_PalyerDc, (BITMAP*)*(m_vechBitMap[STATUS_NOMAL]));
        break;
    case COtherPlayer::STATUS_AIRPLANE:
        SelectObject(m_PalyerDc, (BITMAP*)*(m_vechBitMap[STATUS_AIRPLANE]));
        break;
    case COtherPlayer::STATUS_ZIGZAG:
        SelectObject(m_PalyerDc, (BITMAP*)*(m_vechBitMap[STATUS_ZIGZAG]));
        break;
    }


    if (m_eBeforeStatus == STATUS_END)
    {
        switch (m_eStatus)
        {
        case COtherPlayer::STATUS_NOMAL:
            SelectObject(m_PalyerDc, (BITMAP*)*(m_vechBitMap[STATUS_NOMAL]));
            break;
        case COtherPlayer::STATUS_AIRPLANE:
            SelectObject(m_PalyerDc, (BITMAP*)*(m_vechBitMap[STATUS_AIRPLANE]));
            break;
        case COtherPlayer::STATUS_ZIGZAG:
            SelectObject(m_PalyerDc, (BITMAP*)*(m_vechBitMap[STATUS_ZIGZAG]));
            break;
        }
    }
    else
    {
        switch (m_eBeforeStatus)
        {
        case COtherPlayer::STATUS_NOMAL:
            SelectObject(m_PalyerDc, (BITMAP*)*(m_vechBitMap[STATUS_NOMAL]));
            break;
        case COtherPlayer::STATUS_AIRPLANE:
            SelectObject(m_PalyerDc, (BITMAP*)*(m_vechBitMap[STATUS_AIRPLANE]));
            break;
        case COtherPlayer::STATUS_ZIGZAG:
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

    PlgBlt(_BackDc, m_NowPoint, m_PalyerDc, 0, 0, 128, 128, NULL, 0, 0);

    TransparentBlt(mDC, 0, 0, WINSIZEX, WINSIZEY, _BackDc, 0, 0, WINSIZEX, WINSIZEY, RGB(255, 0, 255));

    DeleteObject(m_CompleteDc);
    DeleteObject(m_PalyerDc);
    DeleteDC(_BackDc);
    DeleteObject(_BackBitmap);

}



void COtherPlayer::Free()
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

void COtherPlayer::KeyDown()
{
}

void COtherPlayer::MSpace(float fTime)
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

float COtherPlayer::radian(float degrees)
{
    return degrees * (PI / 180);
}

void COtherPlayer::LoadPlayerSound()
{
    //CSoundManager::GetInstance()->LoadSound("레벨완료","../sound/Geometry Dash Level Complete - djlunatique.com.mp3");


}

void COtherPlayer::SetTargetPosition(POINT newPos, float newAngle)
{
    _targetPos = newPos;
	_targetAngle = newAngle;
}

//void COtherPlayer::SetStopSpin()
//{
//   m_Jcnt = JumpCnt;
//   angle = -SpinSpeed;
//   m_bMSpace = false;
//}
