#include "Monster.h"
#include "ObjManager.h"
#include "CameraManager.h"

//#pragma comment (lib, "msimg32.lib")
CMonster::CMonster()
{
	Initailizer();
}

CMonster::~CMonster()
{
	Free();
}

void CMonster::Initailizer()
{
	HBITMAP* InBitMap{};

	TCHAR Name[50] = {};
	for (int i = 0; i < 3; ++i)
	{
		wsprintf(Name, L"../resource/Monster/Attak/JumpAttak%d.bmp", i);
		InBitMap = new HBITMAP;
		*InBitMap = (HBITMAP)LoadImage(g_hInst, Name, IMAGE_BITMAP, 128, 128, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		m_vechBitMap[RS_IDLE].push_back(InBitMap);

		wsprintf(Name, L"../resource/Monster/Death/Death%d.bmp", i);
		InBitMap = new HBITMAP;
		*InBitMap = (HBITMAP)LoadImage(g_hInst, Name, IMAGE_BITMAP, 128, 128, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		m_vechBitMap[RS_DEATH].push_back(InBitMap);

	}
}

int CMonster::Update(float fTime)
{
	

	if (m_bDeath == true)
	{
		m_fTime += fTime * 10.f;

		if ((int)m_fTime >= m_vechBitMap[m_NowResource].size())
		{
			m_fTime = 0.f;
			return -1;
		}
		return 0;
	}

	m_fTime += fTime * 25.f;

	if (m_iHp <= 0)
	{
		m_NowResource = RS_DEATH;
		m_bDeath = true;
		m_fTime = 0.f;
		return 0;
	}

	m_POSRECT = RECT{ (long)(m_CenterPos.x - m_fRidius), (long)(m_CenterPos.y - m_fRidius), (long)(m_CenterPos.x + m_fRidius), (long)(m_CenterPos.y + m_fRidius) };



	switch (m_NowDir)
	{
	case CMonster::DIR_LEFT:
		m_CenterPos.x -= m_fSpeed * fTime;
		break;
	case CMonster::DIR_RIGHT:
		m_CenterPos.x += m_fSpeed * fTime;
		break;
	case CMonster::DIR_UP:
		m_CenterPos.y -= m_fSpeed * fTime;
		break;
	case CMonster::DIR_DOWN:
		m_CenterPos.y += m_fSpeed * fTime;
		break;
	}

	return 0;
}

void CMonster::LateUpdate(float fTime)
{
	if ((int)m_fTime >= m_vechBitMap[m_NowResource].size())
	{
		m_fTime = 0.f;
	}
	Collision();
}

void CMonster::Render(HDC mDC)
{
	m_PalyerDc = CreateCompatibleDC(mDC);
	SelectObject(m_PalyerDc, *(m_vechBitMap[m_NowResource][(int)m_fTime]));
	TransparentBlt(mDC, m_CenterPos.x - m_fRidius - CCameraManager::GetInstance()->GetCenterPos().x, m_CenterPos.y - m_fRidius - CCameraManager::GetInstance()->GetCenterPos().y, m_fRidius * 2.f, m_fRidius * 2.f, m_PalyerDc, 0, 0, 128, 128, RGB(255, 255, 255));
	DeleteObject(m_PalyerDc);
}

void CMonster::Free()
{
	for (int i = 0; i < RS_END; ++i)
	{
		if (m_vechBitMap[i].size())
		{
			for (auto iter = m_vechBitMap[i].begin(); iter != m_vechBitMap[i].end(); ++iter)
			{
				DeleteObject(*(*iter));

				delete (*iter);
				(*iter) = nullptr;


			}
			m_vechBitMap[i].clear();
		}
	}
}



void CMonster::Collision()
{
	m_POSRECT = RECT{ (long)(m_CenterPos.x - m_fRidius), (long)(m_CenterPos.y - m_fRidius), (long)(m_CenterPos.x + m_fRidius), (long)(m_CenterPos.y + m_fRidius) };

	if (m_POSRECT.left < 0)
	{
		m_NowDir = DIR_RIGHT;
	}
	if (m_POSRECT.right > 800)
	{
		m_NowDir = DIR_LEFT;
	}

	if (m_POSRECT.top < 0)
	{
		m_NowDir = DIR_DOWN;
	}

	if (m_POSRECT.bottom > 600)
	{
		m_NowDir = DIR_UP;
	}

	for (auto iter = CObjManager::GetInstance()->GetAllVector()[CObjManager::OBJECT_PLAYER].begin();
		iter != CObjManager::GetInstance()->GetAllVector()[CObjManager::OBJECT_PLAYER].end();
		++iter)
	{
		RECT RC = (*iter)->GetPosRect(), NullRect{};
		if (IntersectRect(&NullRect, &RC, &m_POSRECT))
		{
			switch (m_NowDir)
			{
			case CMonster::DIR_LEFT:
				m_NowDir = DIR_RIGHT;
				break;
			case CMonster::DIR_RIGHT:
				m_NowDir = DIR_LEFT;

				break;
			case CMonster::DIR_UP:
				m_NowDir = DIR_DOWN;

				break;
			case CMonster::DIR_DOWN:
				m_NowDir = DIR_UP;

				break;
			}
		}
	}

}
