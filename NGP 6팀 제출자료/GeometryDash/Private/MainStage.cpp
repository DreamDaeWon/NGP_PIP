#include "MainStage.h"
#include "ObjManager.h"
#include "CameraManager.h"
#include "CSoundManager.h"
#include "MainCamera.h"
#include "GoStageButton.h"
#include "GoToolButton.h"


CMainStage::CMainStage()
{
	Initailizer();
}

CMainStage::~CMainStage()
{
	Free();
}

void CMainStage::Initailizer()
{
	CSoundManager::GetInstance()->SetAllvolume(0.f);
	CCameraManager::GetInstance()->SetBackBitMapImage(L"../resource/ToolStageBack/ToolBackImage.bmp", 2048, 2048);
	CCameraManager::GetInstance()->SetBackImageSize(2048, 2048);

	SpawnCamera();
	SpawnButton();
}

void CMainStage::Update(float fTime)
{
	CObjManager::GetInstance()->Update(fTime);
}

void CMainStage::LateUpdate(float fTime)
{
	CObjManager::GetInstance()->LateUpdate(fTime);
}

void CMainStage::Render(HDC mDC)
{
	CObjManager::GetInstance()->Render(mDC);
}

void CMainStage::Free()
{
	CObjManager::GetInstance()->Free();
}

void CMainStage::SpawnCamera()
{
	CObject* Camera = new CMainCamera{};

	CObjManager::GetInstance()->GetAllVector()[CObjManager::OBJECT_CAMERA].push_back(Camera);
}

void CMainStage::SpawnButton()
{
	CObject* BUTTON = new CGoStageButton{};

	BUTTON->SetCenterPos(POINT{ 400, 300 });
	BUTTON->SetRidius(70);

	CObjManager::GetInstance()->GetAllVector()[CObjManager::OBJECT_BUTTON].push_back(BUTTON);

	BUTTON = new CGoToolButton{};

	BUTTON->SetCenterPos(POINT{ 600, 300 });
	BUTTON->SetRidius(50);

	CObjManager::GetInstance()->GetAllVector()[CObjManager::OBECT_ESC_MENU].push_back(BUTTON);
}


