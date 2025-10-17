#include "MainCamera.h"
#include "CameraManager.h"

CMainCamera::CMainCamera()
{
	Initailizer();
}

CMainCamera::~CMainCamera()
{
	Free();
}

void CMainCamera::Initailizer()
{
	m_CenterPos.x = 0;
	m_CenterPos.y = 0;
}

int CMainCamera::Update(float fTime)
{
	//m_CenterPos.x -= 100;
	CCameraManager::GetInstance()->PlusMoveX(1);
	//CCameraManager::GetInstance()->SetCenterPos(m_CenterPos);
	return 0;
}

void CMainCamera::LateUpdate(float fTime)
{

}

void CMainCamera::Render(HDC mDC)
{
}

void CMainCamera::Free()
{
}
