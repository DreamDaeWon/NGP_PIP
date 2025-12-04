#include "OtherPlayer.h"
#include "ObjManager.h"
#include "CameraManager.h"
#include "KeyManager.h"
#include "CollisionManager.h"
#include "Tile.h"
#include "CSoundManager.h"
#include "StageManager.h"
#include <chrono> // Added for time-based prediction

//#pragma comment (lib, "msimg32.lib")
COtherPlayer::COtherPlayer()
{
    _lastPacketTime = chrono::steady_clock::now();
}

COtherPlayer::~COtherPlayer()
{
    Free();
}

void COtherPlayer::Initailizer()
{
   LoadOtherPlayerBitMap(_OtherPlayerID);
}

int COtherPlayer::Update(float fTime)
{
    // 1. 예측 시간 계산
    auto now = chrono::steady_clock::now();
    chrono::duration<float> timeSinceLastPacket = now - _lastPacketTime;
    float dtPrediction = timeSinceLastPacket.count(); // 마지막 패킷 이후 경과 시간

    // 최대 500ms까지만 예측 (네트워크 끊김 방지)
    if (dtPrediction > 0.5f) dtPrediction = 0.5f;

    const float LERP_ALPHA = 15.0f; // Increased for tighter sync

    
    float predictedX = _targetPos.x + (_currentVx * dtPrediction * 1.2f);
    float predictedY = _targetPos.y + (_currentVy * dtPrediction * 1.2f);

	// 3. 위치 보간
    float distSq = pow(predictedX - m_CenterPos.x, 2) + pow(predictedY - m_CenterPos.y, 2);
    if (distSq > 10000.0f) // 100 픽셀 이상 차이나면 순간이동
    {
        m_CenterPos.x = predictedX;
        m_CenterPos.y = predictedY;
    }
    else
    {
        // 부드럽게 보간      
        m_CenterPos.x += (predictedX - m_CenterPos.x) * LERP_ALPHA * fTime;
        m_CenterPos.y += (predictedY - m_CenterPos.y) * LERP_ALPHA * fTime;
    }

    // 4. 회전 각 보간
    if (m_eStatus != STATUS_ZIGZAG || m_eStatus != STATUS_AIRPLANE) 
    {
        float angleDiff = _targetAngle - angle;
        if (angleDiff > 180) angleDiff -= 360;
        else if (angleDiff < -180) angleDiff += 360;
        angle += angleDiff * LERP_ALPHA * fTime;
    }
    else 
    {
        angle = _targetAngle; 
    }

    float ridius{}; 
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


    // ׵θ  ڵ ֱ
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
}

float COtherPlayer::radian(float degrees)
{
    return degrees * (PI / 180);
}

void COtherPlayer::LoadPlayerSound()
{
    //CSoundManager::GetInstance()->LoadSound("레벨완료","../sound/Geometry Dash Level Complete - djlunatique.com.mp3");


}

void COtherPlayer::LoadOtherPlayerBitMap(int id)
{
    HBITMAP* InBitMap{};
    LoadPlayerSound();
    TCHAR Name[50] = {};

    wsprintf(Name, L"../resource/Player/Player%d.bmp", id);
    InBitMap = new HBITMAP;
    *InBitMap = (HBITMAP)LoadImage(g_hInst, Name, IMAGE_BITMAP, 128, 128, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
    m_vechBitMap.push_back(InBitMap);

    wsprintf(Name, L"../resource/Player/Player%d_Ship.bmp", id);
    InBitMap = new HBITMAP;
    *InBitMap = (HBITMAP)LoadImage(g_hInst, Name, IMAGE_BITMAP, 128, 128, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
    m_vechBitMap.push_back(InBitMap);

    wsprintf(Name, L"../resource/Player/Player%d_ZigZag.bmp", id);
    InBitMap = new HBITMAP;
    *InBitMap = (HBITMAP)LoadImage(g_hInst, Name, IMAGE_BITMAP, 128, 128, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
    m_vechBitMap.push_back(InBitMap);
}

void COtherPlayer::SetTargetPosition(POINT newPos, float newVx, float newVy, float newAngle)
{
    _targetPos = newPos;
	_currentVx = newVx;
	_currentVy = newVy;
	_targetAngle = newAngle;
	_lastPacketTime = chrono::steady_clock::now();
}

//void COtherPlayer::SetStopSpin()
//{
//   m_Jcnt = JumpCnt;
//   angle = -SpinSpeed;
//   m_bMSpace = false;
//}