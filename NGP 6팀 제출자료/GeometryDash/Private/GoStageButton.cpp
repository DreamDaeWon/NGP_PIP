#include "GoStageButton.h"
#include "CSoundManager.h"
#include "ObjManager.h"
#include "CollisionManager.h"
#include "KeyManager.h"
#include "StageManager.h"

CGoStageButton::CGoStageButton()
{
    Initailizer();
}

CGoStageButton::~CGoStageButton()
{
    Free();
}

void CGoStageButton::Initailizer()
{
    HBITMAP* InBitMap{};
    TCHAR Name[50] = {};

    wsprintf(Name, L"../resource/Button/PlayButton1.bmp");
    InBitMap = new HBITMAP;
    *InBitMap = (HBITMAP)LoadImage(g_hInst, Name, IMAGE_BITMAP, 128, 128, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
    m_vechBitMap.push_back(InBitMap);

    wsprintf(Name, L"../resource/Button/Name.bmp");
    InBitMap = new HBITMAP;
    *InBitMap = (HBITMAP)LoadImage(g_hInst, Name, IMAGE_BITMAP, 500, 54, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
    m_vechBitMap.push_back(InBitMap);
    wsprintf(Name, L"../resource/MainMenu/MadeBy.bmp");
    InBitMap = new HBITMAP;
    *InBitMap = (HBITMAP)LoadImage(g_hInst, Name, IMAGE_BITMAP, 800, 600, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
    m_vechBitMap.push_back(InBitMap);
}

int CGoStageButton::Update(float fTime)
{
    return 0;
}

void CGoStageButton::LateUpdate(float fTime)
{
    m_POSRECT.left = m_CenterPos.x - m_fRidius;
    m_POSRECT.right = m_CenterPos.x + m_fRidius;
    m_POSRECT.top = m_CenterPos.y - m_fRidius;
    m_POSRECT.bottom = m_CenterPos.y + m_fRidius;
    POINT Mouse{};
    GetCursorPos(&Mouse);
    ScreenToClient(g_hWnd, &Mouse);


    if (CCollisionManager::GetInstance()->CheckCircle_And_Point(this, Mouse) && CKeyManager::GetInstance()->KeyUp(VK_LBUTTON))
    {
        CStageManager::GetInstance()->StageChanger(CStageManager::STAGE_MENU);
    }
}

void CGoStageButton::Render(HDC mDC)
{
    m_ButtonDc = CreateCompatibleDC(mDC);
    m_NameDc = CreateCompatibleDC(mDC);
    m_MadeByDc = CreateCompatibleDC(mDC);

    SelectObject(m_ButtonDc, *(m_vechBitMap[0]));
    SelectObject(m_NameDc, *(m_vechBitMap[1]));
    SelectObject(m_MadeByDc, *(m_vechBitMap[2]));

    TransparentBlt(mDC, m_CenterPos.x - m_fRidius,
        m_CenterPos.y - m_fRidius,
        m_fRidius * 2, m_fRidius * 2, m_ButtonDc, 0, 0, 128, 128, RGB(255, 0, 255));

    TransparentBlt(mDC, m_CenterPos.x - 250,
        m_CenterPos.y - 150 - m_fRidius,
        500, 54, m_NameDc, 0, 0, 500, 54, RGB(255, 0, 255));

    TransparentBlt(mDC, 0,
        0,
        800, 600, m_MadeByDc, 0, 0, 800, 600, RGB(132, 126, 135));

    DeleteObject(m_ButtonDc);
    DeleteObject(m_NameDc);
    DeleteObject(m_MadeByDc);
}

void CGoStageButton::Free()
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
