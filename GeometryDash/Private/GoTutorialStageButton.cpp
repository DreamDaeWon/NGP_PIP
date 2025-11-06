#include "GoTutorialStageButton.h"
#include "CSoundManager.h"
#include "ObjManager.h"
#include "CollisionManager.h"
#include "KeyManager.h"
#include "StageManager.h"


CGoTutorialStageButton::CGoTutorialStageButton()
{
    Initailizer();
}

CGoTutorialStageButton::~CGoTutorialStageButton()
{
    Free();
}

void CGoTutorialStageButton::Initailizer()
{
    HBITMAP* InBitMap{};
    TCHAR Name[50] = {};

    wsprintf(Name, L"../resource/Mainmenu/TutorialButton.bmp");
    //wsprintf(Name, L"../resource/Player/Gatto_Roll_%d.bmp",i);
    InBitMap = new HBITMAP;
    *InBitMap = (HBITMAP)LoadImage(g_hInst, Name, IMAGE_BITMAP, 128, 128, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
    m_vechBitMap.push_back(InBitMap);
    CSoundManager::GetInstance()->LoadSound("SelectSatage", "../sound/geometry-dash-level-selected.mp3");
}

int CGoTutorialStageButton::Update(float fTime)
{
    m_fTime += fTime * 10.f;
    if (angle < 360) {
        fY += 0.5f * sin(radian(angle));
        angle = angle + 5 * m_fTime;
    }
    else {
        angle = 0;
    }
    return 0;
}

void CGoTutorialStageButton::LateUpdate(float fTime)
{

    if ((int)m_fTime >= m_vechBitMap.size())
    {
        m_fTime = 0.f;
    }

    m_POSRECT.left = m_CenterPos.x - m_fRidius;
    m_POSRECT.right = m_CenterPos.x + m_fRidius;
    m_POSRECT.top = m_CenterPos.y - m_fRidius;
    m_POSRECT.bottom = m_CenterPos.y + m_fRidius;
    POINT Mouse{};
    GetCursorPos(&Mouse);
    ScreenToClient(g_hWnd, &Mouse);


    if (CCollisionManager::GetInstance()->CheckCircle_And_Point(this, Mouse) && CKeyManager::GetInstance()->KeyDown(VK_LBUTTON))
    {
        CSoundManager::GetInstance()->SetAllvolume(0.f);

        CSoundManager::GetInstance()->PlayEffectSound("SelectSatage", CSoundManager::CHANNEL_OBJECT_SOUND1);
        CSoundManager::GetInstance()->SetVolume(CSoundManager::CHANNEL_OBJECT_SOUND1, 0.5f);
        Sleep(1300);
        CStageManager::GetInstance()->StageChanger(CStageManager::STAGE_TUTORIAL);
    }
}

void CGoTutorialStageButton::Render(HDC mDC)
{
    m_ButtonDc = CreateCompatibleDC(mDC);

    SelectObject(m_ButtonDc, (BITMAP*)*(m_vechBitMap[0]));

    TransparentBlt(mDC, m_CenterPos.x - m_fRidius,
        fY - (m_fRidius) + m_CenterPos.y,
        m_fRidius * 2, m_fRidius * 2, m_ButtonDc, 0, 0, 128, 128, RGB(10, 10, 10));

    DeleteObject(m_ButtonDc);
}

void CGoTutorialStageButton::Free()
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

float CGoTutorialStageButton::radian(float degrees)
{
    return degrees * (PI / 180);
}

