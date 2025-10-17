#include "ToolStage.h"
#include "CameraManager.h"
#include "ObjManager.h"
#include "ToolCamera.h"
#include "ToolMouse.h"
#include "CSoundManager.h"


#include "GoToolButton.h"
#include "ResumeButton.h"
#include "GoBackButton.h"


CToolStage::CToolStage()
{
	Initailizer();
}

CToolStage::~CToolStage()
{
	Free();
}

void CToolStage::Initailizer()
{
	// 배경음 설정
	CSoundManager::GetInstance()->SetAllvolume(0.f);

	// 카메라 생성
	CreatCamera();
	// 마우스 생성
	CreatMouse();
	/*CSoundManager::GetInstance()->LoadSound("GeometryDashMenuTheme", "../sound/GeometryDash.mp3");
	CSoundManager::GetInstance()->PlayBGM("GeometryDashMenuTheme");
	CSoundManager::GetInstance()->SetVolume(CSoundManager::CHANNEL_BGM, 0.1f);*/


	// 배경 설정
	CCameraManager::GetInstance()->SetBackBitMapImage(L"../resource/ToolStageBack/ToolBackImage.bmp", 2048, 2048);
	CCameraManager::GetInstance()->SetBackImageSize(20480,2048);
	CCameraManager::GetInstance()->SetNomalLeftRight();




}

void CToolStage::Update(float fTime)
{
	CObjManager::GetInstance()->Update(fTime);
}

void CToolStage::LateUpdate(float fTime)
{
	CObjManager::GetInstance()->LateUpdate(fTime);
}

void CToolStage::Render(HDC mDC)
{
	// 그리드 그리기
	PrintGrid(mDC);

	CObjManager::GetInstance()->Render(mDC);
}

void CToolStage::Free()
{
	CObjManager::GetInstance()->Free();
}

void CToolStage::PrintGrid(HDC mDC)
{
	HPEN hPen = CreatePen(PS_SOLID,1,RGB(0,0,0)), OldPen;
	OldPen = (HPEN)SelectObject(mDC, hPen);

	// 세로 선
	for (int j= -CCameraManager::GetInstance()->GetCenterPos().x; j < CCameraManager::GetInstance()->GetBackImageSize().x + iGridSizeX - CCameraManager::GetInstance()->GetCenterPos().x; j += iGridSizeX)
	{
		MoveToEx(mDC,j, 0, nullptr);
		LineTo(mDC,j, CCameraManager::GetInstance()->GetBackImageSize().y);
	}

	//가로 선
	for (int i = -CCameraManager::GetInstance()->GetCenterPos().y; i < CCameraManager::GetInstance()->GetBackImageSize().y + iGridSizeY - CCameraManager::GetInstance()->GetCenterPos().y; i+= iGridSizeY)
	{
		MoveToEx(mDC, 0, i, nullptr);
		LineTo(mDC, CCameraManager::GetInstance()->GetBackImageSize().x, i);
	}

	SelectObject(mDC, OldPen);
	DeleteObject(hPen);

}

void CToolStage::CreatCamera()
{
	CObject* Camera = new CToolCamera{};

	CObjManager::GetInstance()->GetAllVector()[CObjManager::OBJECT_CAMERA].push_back(Camera);


}

void CToolStage::CreatMouse()
{
	CObject* Mouse = new CToolMouse{};

	CObjManager::GetInstance()->GetAllVector()[CObjManager::OBJECT_MOUSE].push_back(Mouse);

}

void CToolStage::CreatEscMenu()
{
	CObject* GoToolMenu = new CResumeButton{};

	GoToolMenu->SetCenterPos(POINT{ 200, 300 });
	GoToolMenu->SetRidius(100);

	CObjManager::GetInstance()->GetAllVector()[CObjManager::OBECT_ESC_MENU].push_back(GoToolMenu);

	CObject* GoMenu = new CGoBackButton{};

	GoMenu->SetCenterPos(POINT{ 600, 300 });
	GoMenu->SetRidius(100);

	CObjManager::GetInstance()->GetAllVector()[CObjManager::OBECT_ESC_MENU].push_back(GoMenu);
}




