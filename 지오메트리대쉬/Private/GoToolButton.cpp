#include "GoToolButton.h"
#include "CSoundManager.h"
#include "ObjManager.h"
#include "CollisionManager.h"
#include "KeyManager.h"
#include "StageManager.h"

CGoToolButton::CGoToolButton()
{
	Initailizer();
}

CGoToolButton::~CGoToolButton()
{
    Free();
}

void CGoToolButton::Initailizer()
{
    LoadButtonSound();


    HBITMAP* InBitMap{};
    TCHAR Name[50] = {};

    wsprintf(Name, L"../resource/Button/ToolButton.bmp");
    //wsprintf(Name, L"../resource/Player/Gatto_Roll_%d.bmp",i);
    InBitMap = new HBITMAP;
    *InBitMap = (HBITMAP)LoadImage(g_hInst, Name, IMAGE_BITMAP, 128, 128, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
    m_vechBitMap.push_back(InBitMap);
}

int CGoToolButton::Update(float fTime)
{
 
    return 0;
}

void CGoToolButton::LateUpdate(float fTime)
{
    m_POSRECT.left = m_CenterPos.x - m_fRidius;
    m_POSRECT.right = m_CenterPos.x + m_fRidius;
    m_POSRECT.top = m_CenterPos.y - m_fRidius;
    m_POSRECT.bottom = m_CenterPos.y + m_fRidius;
    POINT Mouse{};
    GetCursorPos(&Mouse);
    ScreenToClient(g_hWnd, &Mouse);


    if (CCollisionManager::GetInstance()->CheckCircle_And_Point(this, Mouse) && CKeyManager::GetInstance()->KeyDown(VK_LBUTTON))
    {
        CStageManager::GetInstance()->StageChanger(CStageManager::STAGE_TOOL);
    }
}

void CGoToolButton::Render(HDC mDC)
{
    m_ButtonDc = CreateCompatibleDC(mDC);

    SelectObject(m_ButtonDc, *(m_vechBitMap[0]));

    TransparentBlt(mDC, m_CenterPos.x- m_fRidius,
        m_CenterPos.y- m_fRidius,
        m_fRidius*2, m_fRidius*2, m_ButtonDc, 0, 0, 128, 128, RGB(255, 0, 255));

    DeleteObject(m_ButtonDc);
}

void CGoToolButton::Free()
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

void CGoToolButton::LoadButtonSound()
{
     CSoundManager::GetInstance()->LoadSound("¹öÆ°","../sound/geometry-dash-level-selected.mp3");
}
