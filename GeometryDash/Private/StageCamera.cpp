#include "StageCamera.h"
#include "KeyManager.h"
#include "CameraManager.h"
#include "ObjManager.h"
#include "Object.h"
#include "Player.h"

CStageCamera::CStageCamera()
{
	Initailizer();
	
}

CStageCamera::~CStageCamera()
{
	Free();
}

void CStageCamera::Initailizer()
{
	m_CenterPos.x = 0;
	m_CenterPos.y = 0;
}

int CStageCamera::Update(float fTime)
{
	CObject* pPlayer = CObjManager::GetInstance()->GetAllVector()[CObjManager::OBJECT_PLAYER].front();


	POINT pPlayerPoint = pPlayer->GetCenterPos();

	if (dynamic_cast<CPlayer*>(pPlayer)->Get_Status() == CPlayer::STATUS_FINISH)
	{
		return 0;
	}

	if (pPlayerPoint.x - m_CenterPos.x > 300)
	{
		m_CenterPos.x += pPlayerPoint.x - m_CenterPos.x - 300;
		CCameraManager::GetInstance()->PlusMoveX(1);
	}

	if (pPlayerPoint.x - m_CenterPos.x < 200)
	{
		m_CenterPos.x =  pPlayerPoint.x - 200;
		CCameraManager::GetInstance()->PlusMoveX(-1);
	}

	if (pPlayerPoint.y - m_CenterPos.y < 100)
	{
		m_CenterPos.y = m_CenterPos.y - (100 - (pPlayerPoint.y - m_CenterPos.y));
	}

	if (pPlayerPoint.y - m_CenterPos.y > 400)
	{
		m_CenterPos.y = m_CenterPos.y + ((pPlayerPoint.y - m_CenterPos.y) - 400);
	}

	if (m_CenterPos.x < 0)
	{
		m_CenterPos.x = 0;
	}
	if (m_CenterPos.x > CCameraManager::GetInstance()->GetBackImageSize().x - WINSIZEX)
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

void CStageCamera::LateUpdate(float fTime)
{
	//CCameraManager::GetInstance()->

}

void CStageCamera::Render(HDC mDC)
{

}

void CStageCamera::Free()
{

}
