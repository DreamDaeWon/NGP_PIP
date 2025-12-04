#include "CameraManager.h"

CCameraManager* CCameraManager::m_pCameraManager = nullptr;

CCameraManager::CCameraManager()
{
	Initailizer();
}

CCameraManager::~CCameraManager()
{
	Free();
	
}

void CCameraManager::Initailizer()
{
	SetPoint(InPointOrigin);
}

int CCameraManager::Update(float fTime)
{
	m_fTime += fTime;

	switch (m_eNowState)
	{
	case CCameraManager::STATE_NONE:
		
		break;
	case CCameraManager::STATE_CHANGE_LEFTRIGHT:
		ChangeLeftRight(fTime);
		break;
	}


    return 0;
}

void CCameraManager::LateUpdate(float fTime)
{
}

void CCameraManager::Render(HDC mDC)
{

}

void CCameraManager::Free()
{
	if (m_hBitmapBackGround != NULL)
	{
		DeleteObject(m_hBitmapBackGround);
	}
}

void CCameraManager::Render(HDC hDC, HDC mDC)
{
	// 모든 오브젝트를 그림

	// 배경 먼저 그리기
	m_hBackImageDC = CreateCompatibleDC(m_BackDc);
	SelectObject(m_hBackImageDC, m_hBitmapBackGround);
	if (m_CenterPos.y <= 0)
	{
		m_CenterPos.y = 0;
	}
	else if(m_BackImageOriginalSize.y - m_CenterPos.y <= WINSIZEY)
	{
		m_CenterPos.y = m_BackImageOriginalSize.y - WINSIZEY;
	}

	if (m_BackImageMove.x >= m_BackImageOriginalSize.x + WINSIZEX) // 너무가면 되돌아오기
	{
		m_BackImageMove.x = 0;
	}

	if (m_BackImageMove.x >= m_BackImageOriginalSize.x - WINSIZEX) // 넘어갔을 때
	{
		/*m_BackImageMove.x = m_BackImageOriginalSize.x - WINSIZEX;*/
		TransparentBlt(m_BackDc, m_BackImageOriginalSize.x - m_BackImageMove.x, 0, WINSIZEX, WINSIZEY, m_hBackImageDC, m_BackImageOriginalSize.x - WINSIZEX, m_CenterPos.y,WINSIZEX, WINSIZEY, RGB(255, 0, 255));
		TransparentBlt(m_BackDc, m_BackImageOriginalSize.x - WINSIZEX - m_BackImageMove.x, 0, WINSIZEX, WINSIZEY, m_hBackImageDC, 0, m_CenterPos.y, WINSIZEX, WINSIZEY, RGB(255, 0, 255));
	}
	else
	{
		TransparentBlt(m_BackDc, 0, 0, WINSIZEX, WINSIZEY, m_hBackImageDC, m_BackImageMove.x, m_CenterPos.y, WINSIZEX, WINSIZEY, RGB(255, 0, 255));
		TransparentBlt(m_BackDc, m_BackImageOriginalSize.x - m_BackImageMove.x, 0, WINSIZEX, WINSIZEY, m_hBackImageDC, 0, m_CenterPos.y, WINSIZEX, WINSIZEY, RGB(255, 0, 255));
		
	}
	
	DeleteObject(m_hBackImageDC);



	PlgBlt(mDC, m_NowPoint, mDC, 0, 0, WINSIZEX, WINSIZEY, NULL, 0, 0);

	switch (m_eNowState)
	{
	case CCameraManager::STATE_NONE:
		if (m_eBeforeState == STATE_CHANGE_LEFTRIGHT)
		{
			if (m_NowPoint[0].x <= WINSIZEX * 0.5f)
			{
				TransparentBlt(m_BackDc, m_NowPoint[0].x, 0, abs(m_NowPoint[0].x - m_NowPoint[1].x), WINSIZEY, mDC, m_NowPoint[0].x, 0, abs(m_NowPoint[0].x - m_NowPoint[1].x), WINSIZEY, RGB(255, 0, 255));
			}
			else
			{
				TransparentBlt(m_BackDc, m_NowPoint[1].x, 0, abs(m_NowPoint[0].x - m_NowPoint[1].x), WINSIZEY, mDC, m_NowPoint[1].x, 0, abs(m_NowPoint[0].x - m_NowPoint[1].x), WINSIZEY, RGB(255, 0, 255));
			}
		}
		else if (m_eBeforeState == STATE_NONE)
		{
			TransparentBlt(m_BackDc, 0, 0, WINSIZEX, WINSIZEY, mDC, 0, 0, WINSIZEX, WINSIZEY, RGB(255, 0, 255));
		}
		break;
	case CCameraManager::STATE_CHANGE_LEFTRIGHT:
		if (m_NowPoint[0].x <= WINSIZEX * 0.5f)
		{
			TransparentBlt(m_BackDc, m_NowPoint[0].x, 0, abs(m_NowPoint[0].x - m_NowPoint[1].x), WINSIZEY, mDC, m_NowPoint[0].x, 0, abs(m_NowPoint[0].x - m_NowPoint[1].x), WINSIZEY, RGB(255, 0, 255));
		}
		else
		{
			TransparentBlt(m_BackDc, m_NowPoint[1].x, 0, abs(m_NowPoint[0].x - m_NowPoint[1].x), WINSIZEY, mDC, m_NowPoint[1].x, 0, abs(m_NowPoint[0].x - m_NowPoint[1].x), WINSIZEY, RGB(255, 0, 255));
		}
		break;
	}



	// 최종
	PlgBlt(hDC, InPointOrigin, m_BackDc, 0, 0, WINSIZEX, WINSIZEY, NULL, 0, 0);

}

void CCameraManager::SetBackDC(HDC _BackDc)
{
	m_BackDc = _BackDc;
}

void CCameraManager::SetBackBitMapImage(const TCHAR* _Name, int cx , int cy)
{
	if (m_hBitmapBackGround != NULL)
	{
		DeleteObject(m_hBitmapBackGround);
	}
	m_hBitmapBackGround = (HBITMAP)LoadImage(g_hInst, _Name, IMAGE_BITMAP, cx, cy, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
	m_BackImageOriginalSize.x = cx;
	m_BackImageOriginalSize.y = cy;

}

void CCameraManager::SetChangeLeftRight()
{
	m_bChangeLeftRight = true;
	m_fTime = 0.f;
	m_eBeforeState = m_eNowState;
	m_eNowState = STATE_CHANGE_LEFTRIGHT;

	if (m_bChangeLeftRight)
	{
		SetPoint(InPointOrigin);
	}
	else
	{
		SetPoint(InPointChangeLeftRight);
	}
}

void CCameraManager::SetNomalLeftRight()
{
	m_bChangeLeftRight = false;
	m_fTime = 0.f;
	m_eBeforeState = m_eNowState;
	m_eNowState = STATE_CHANGE_LEFTRIGHT;

	if (m_bChangeLeftRight)
	{
		SetPoint(InPointOrigin);
	}
	else
	{
		SetPoint(InPointChangeLeftRight);
	}
}

void CCameraManager::ChangeLeftRight(float fTime)
{
	float iChangeSpeed = {2000.f};

	if (m_bChangeLeftRight)
	{
		m_NowPoint[0].x += fTime * iChangeSpeed;
		m_NowPoint[1].x -= fTime * iChangeSpeed;
		m_NowPoint[2].x += fTime * iChangeSpeed;

		if (m_NowPoint[0].x >= WINSIZEX)
		{
			m_NowPoint[0].x = WINSIZEX;
			m_NowPoint[1].x = 0;
			m_NowPoint[2].x = WINSIZEX;
		}
	}
	else
	{
		m_NowPoint[0].x -= fTime * iChangeSpeed;
		m_NowPoint[1].x += fTime * iChangeSpeed;
		m_NowPoint[2].x -= fTime * iChangeSpeed;

		if (m_NowPoint[0].x <= 0)
		{
			m_NowPoint[0].x = 0;
			m_NowPoint[1].x = WINSIZEX;
			m_NowPoint[2].x = 0;
		}
	}


}
