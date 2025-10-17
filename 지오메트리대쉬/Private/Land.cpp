#include "Land.h"

CLand::CLand()
{
	Initailizer();
}

CLand::~CLand()
{
	Free();
}

void CLand::Initailizer()
{
    HBITMAP* InBitMap{};
    TCHAR Name[50] = {};
    wsprintf(Name, L"../resource/Mainmenu/Mainmenu.bmp");
    InBitMap = new HBITMAP;
    *InBitMap = (HBITMAP)LoadImage(g_hInst, Name, IMAGE_BITMAP, 800, 600, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
    m_vechBitMap.push_back(InBitMap);
}

int CLand::Update(float fTime)
{
    m_fTime += fTime * 10.f;
    if (angle < 360) {
        fY += 0.5f*sin(radian(angle));
        angle = angle + 5* m_fTime;
    }
    else {
        angle = 0;
    }
   /* switch (m_NowDir)
    {
    case CLand::DIR_UP:
        
    break;
    case CLand::DIR_DOWN:
        break;
    }*/
    return 0;
}

void CLand::LateUpdate(float fTime)
{
    if ((int)m_fTime >= m_vechBitMap.size())
    {
        m_fTime = 0.f;
    }
    Collision();

}

void CLand::Render(HDC mDC)
{
    m_LandDc = CreateCompatibleDC(mDC);
    SelectObject(m_LandDc, (BITMAP*)*(m_vechBitMap[0]));

    TransparentBlt(mDC, m_CenterPos.x - m_fRidius, fY - (m_fRidius)+m_CenterPos.y, WINSIZEX, WINSIZEY, m_LandDc, 0, 0, WINSIZEX, WINSIZEY, RGB(0, 0, 0));

    DeleteObject(m_LandDc);
}

void CLand::Free()
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

void CLand::Collision()
{
   /* m_POSRECT = RECT{ (long)(m_CenterPos.x - m_fRidius), (long)(m_CenterPos.y - (m_fRidius / 2.f)), (long)(m_CenterPos.x + m_fRidius), (long)(m_CenterPos.y + (m_fRidius / 2.f)) };

    if (m_POSRECT.top < 100)
    {
        m_NowDir = DIR_DOWN;
    }
    if (m_POSRECT.bottom >= 600)
    {
        m_NowDir = DIR_UP;
    }*/
}

float CLand::radian(float degrees)
{
     return degrees * (PI / 180);
}
