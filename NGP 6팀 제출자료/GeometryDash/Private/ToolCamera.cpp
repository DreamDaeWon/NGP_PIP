#include "ToolCamera.h"
#include "KeyManager.h"
#include "CameraManager.h"

CToolCamera::CToolCamera()
{
	Initailizer();
}

CToolCamera::~CToolCamera()
{
	Free();
}

void CToolCamera::Initailizer()
{
	//m_CenterPos.x = WINSIZEX * 0.5f;
	//m_CenterPos.y = WINSIZEY * 0.5f;
}

int CToolCamera::Update(float fTime)
{
	if (CKeyManager::GetInstance()->KeyPressing(VK_RIGHT))
	{
		m_CenterPos.x += (int)(fTime * 100.f * m_fSpeed);
		CCameraManager::GetInstance()->PlusMoveX(1);
	}

	if (CKeyManager::GetInstance()->KeyPressing(VK_LEFT))
	{

		m_CenterPos.x -= (int)(fTime * 100.f * m_fSpeed);
		CCameraManager::GetInstance()->PlusMoveX(-1);
	}

	if (CKeyManager::GetInstance()->KeyPressing(VK_UP))
	{
		m_CenterPos.y -= (int)(fTime * 100.f * m_fSpeed);
	}

	if (CKeyManager::GetInstance()->KeyPressing(VK_DOWN))
	{
		m_CenterPos.y += (int)(fTime * 100.f * m_fSpeed);
	}

	if (m_CenterPos.x < 0)
	{
		m_CenterPos.x = 0;
	}
	 if(m_CenterPos.x > CCameraManager::GetInstance()->GetBackImageSize().x - WINSIZEX)
	 { 
		 m_CenterPos.x = CCameraManager::GetInstance()->GetBackImageSize().x - WINSIZEX;
	 }

	 if (m_CenterPos.y < 0)
	 {
		 m_CenterPos.y = 0;
	 }
	 if (m_CenterPos.y > CCameraManager::GetInstance()->GetBackImageSize().y - WINSIZEY)
	 {
		 m_CenterPos.y = CCameraManager::GetInstance()->GetBackImageSize().y - WINSIZEY;
	 }


	CCameraManager::GetInstance()->SetCenterPos(m_CenterPos);

	return 0;
}

void CToolCamera::LateUpdate(float fTime)
{
	//CCameraManager::GetInstance()->

}

void CToolCamera::Render(HDC mDC)
{

}

void CToolCamera::Free()
{

}
