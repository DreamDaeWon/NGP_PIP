#include "MainGame.h"
#include "Object.h"
#include "ObjManager.h"
#include "Player.h"
#include "StageOne.h"
#include "ToolStage.h"
#include "ToolPlayStage.h"
#include "CameraManager.h"
#include "KeyManager.h"
#include "CollisionManager.h"
#include "CSoundManager.h"
#include "StageManager.h"


CMainGame::CMainGame()
{
	Initailizer();
}

CMainGame::~CMainGame()
{
	Free();
}

void CMainGame::Initailizer()
{
	// �Ŵ��� ����
	CCameraManager::GetInstance();
	CObjManager::GetInstance();
	CKeyManager::GetInstance();
	CCollisionManager::GetInstance();
	CSoundManager::GetInstance();
	CStageManager::GetInstance();
	
	//NowStage = new CToolStage{};
}

void CMainGame::Update(float fTime)
{

	CStageManager::GetInstance()->Update(fTime);
		
	CCameraManager::GetInstance()->Update(fTime);

}

void CMainGame::LateUpdate(float fTime)
{

	CStageManager::GetInstance()->LateUpdate(fTime);

	CCameraManager::GetInstance()->LateUpdate(fTime);
}

void CMainGame::Render(HDC mDC)
{

	 CStageManager::GetInstance()->Render(mDC);

	// ������ �� �������� ���־�� ��
	CKeyManager::GetInstance()->UpdateBeforeKey();
}

void CMainGame::Free()
{
	// �Ŵ��� ����
	CStageManager::DestoryInstance();
	CObjManager::DestoryInstance();
	CCameraManager::DestoryInstance();
	CKeyManager::DestoryInstance();
	CCollisionManager::DestoryInstance();
	CSoundManager::DestoryInstance();
}