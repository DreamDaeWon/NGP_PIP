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
    // Calculate elapsed time since last packet for prediction
    auto now = chrono::steady_clock::now();
    chrono::duration<float> timeSinceLastPacket = now - _lastPacketTime;
    float dtPrediction = timeSinceLastPacket.count();

    // Cap prediction time to prevent massive overshooting during lag spikes (e.g., max 500ms)
    if (dtPrediction > 0.5f) dtPrediction = 0.5f;

    const float LERP_ALPHA = 8.0f; // Adjust this value for desired smoothness vs. responsiveness

    // Predict current position based on last received authoritative position and velocity
    // This is the point we want to move towards
    float predictedX = _targetPos.x + (_currentVx * dtPrediction);
    float predictedY = _targetPos.y + (_currentVy * dtPrediction);
    
    // Teleport Threshold: If the discrepancy is too large, snap immediately
    float distSq = pow(predictedX - m_CenterPos.x, 2) + pow(predictedY - m_CenterPos.y, 2);
    if (distSq > 10000.0f) // 100 pixels squared
    {
        m_CenterPos.x = predictedX;
        m_CenterPos.y = predictedY;
    }
    else
    {
        // Smoothly interpolate towards the predicted position.
        // Use a fixed interpolation factor that is applied per second (time-based)
        // A common value for `lerpAlpha` is between 5.0f and 10.0f for responsive smoothing.
        m_CenterPos.x += (predictedX - m_CenterPos.x) * LERP_ALPHA * fTime;
        m_CenterPos.y += (predictedY - m_CenterPos.y) * LERP_ALPHA * fTime;
    }

    // Angle interpolation (time-based)
    if (m_eStatus != STATUS_ZIGZAG || m_eStatus != STATUS_AIRPLANE) // Only interpolate angle if not in AIRPLANE status
    {
        float angleDiff = _targetAngle - angle;
        if (angleDiff > 180) angleDiff -= 360;
        else if (angleDiff < -180) angleDiff += 360;
        angle += angleDiff * LERP_ALPHA * fTime;
    }
    else // If in AIRPLANE status, snap to the target angle or set to a default (e.g. 0)
    {
        angle = _targetAngle; // Use the target angle directly from the server
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