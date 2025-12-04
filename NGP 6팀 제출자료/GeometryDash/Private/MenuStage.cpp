#include "MenuStage.h"
#include "ObjManager.h"
#include "CameraManager.h"
#include "StageCamera.h"
#include "Object.h"
#include "CSoundManager.h"
#include "GotutorialStageButton.h"
#include "GoOneStageButton.h"
#include "Land.h"

#include "GoBackButton.h"

CMenuStage::CMenuStage()
{
	Initailizer();
}

CMenuStage::~CMenuStage()
{
	Free();
}

void CMenuStage::Initailizer()
{
	CSoundManager::GetInstance()->LoadSound("GeometryDashMenuTheme", "../sound/GeometryDash.mp3");
	CSoundManager::GetInstance()->PlayBGM("GeometryDashMenuTheme");
	CSoundManager::GetInstance()->SetVolume(CSoundManager::CHANNEL_BGM, 0.1f);
	SpawnLand();
	CCameraManager::GetInstance()->SetBackBitMapImage(L"../resource/ToolStageBack/ToolBackImage.bmp", 2048, 2048);
	CCameraManager::GetInstance()->SetBackImageSize(800, 600);

	HBITMAP* InBitMap{};
	TCHAR Name[50] = {};

	wsprintf(Name, L"../resource/Mainmenu/LandText.bmp");
	InBitMap = new HBITMAP;
	*InBitMap = (HBITMAP)LoadImage(g_hInst, Name, IMAGE_BITMAP, 800, 600, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
	m_vechBitMap.push_back(InBitMap);
}

void CMenuStage::Update(float fTime)
{
	CObjManager::GetInstance()->Update(fTime);
}

void CMenuStage::LateUpdate(float fTime)
{
	CObjManager::GetInstance()->LateUpdate(fTime);
}

void CMenuStage::Render(HDC mDC)
{
	CObjManager::GetInstance()->Render(mDC);


	m_APlusDC = CreateCompatibleDC(mDC);
	
	SelectObject(m_APlusDC, *(m_vechBitMap[0]));
	
	TransparentBlt(mDC, 0,
		0,
		800, 600, m_APlusDC, 0, 0, 800, 600, RGB(132, 126, 135));
	
	DeleteObject(m_APlusDC);
}

void CMenuStage::Free()
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
	CObjManager::GetInstance()->Free();
}


void CMenuStage::SpawnLand()
{
	CObject* pLand = new CLand{};
	CObjManager::GetInstance()->GetAllVector()[CObjManager::OBJECT_LAND].push_back(pLand);
	POINT Pos{ 400,400 };
	pLand->SetCenterPos(Pos);
	pLand->SetRidius(400.f);

	CObject* ptutorialbutton = new CGoTutorialStageButton{};
	CObjManager::GetInstance()->GetAllVector()[CObjManager::OBJECT_BUTTON].push_back(ptutorialbutton);
	Pos = { 200,340 };
	ptutorialbutton->SetCenterPos(Pos);
	ptutorialbutton->SetRidius(25.f);

	CObject* ponebutton = new CGoOneStageButton{};
	CObjManager::GetInstance()->GetAllVector()[CObjManager::OBJECT_BUTTON].push_back(ponebutton);
	Pos = { 400,300 };
	ponebutton->SetCenterPos(Pos);
	ponebutton->SetRidius(25.f);


	CObject* GoMenu = new CGoBackButton{};

	GoMenu->SetCenterPos(POINT{ 750, 40 });
	GoMenu->SetRidius(20);

	CObjManager::GetInstance()->GetAllVector()[CObjManager::OBECT_ESC_MENU].push_back(GoMenu);
}

