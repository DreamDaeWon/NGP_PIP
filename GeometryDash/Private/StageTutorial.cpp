#include "StageTutorial.h"
#include "ObjManager.h"
#include "Player.h"
#include "Monster.h"
#include "CameraManager.h"
#include "StageCamera.h"
#include "Tile.h"
#include "Object.h"
#include "CSoundManager.h"
#include "KeyManager.h"
#include "StageManager.h"


#include <fstream>
#include <sstream>


#include "GoToolButton.h"
#include "ResumeButton.h"
#include "GoBackButton.h"


CStageTutorial::CStageTutorial()
{
	Initailizer();
}

CStageTutorial::~CStageTutorial()
{
	Free();
}

void CStageTutorial::Initailizer()
{

	SpawnCamera();
	SpawnPlayer();
	SpawnTile();

	// 배경음 설정
	CSoundManager::GetInstance()->LoadSound("TheoryOfEverything", "../sound/TheoryOfEverything.mp3");
	CSoundManager::GetInstance()->PlayBGM("TheoryOfEverything");
	CSoundManager::GetInstance()->SetVolume(0, 0.1f);


	// 배경 사진 및 배경 크기 설정
	CCameraManager::GetInstance()->SetBackBitMapImage(L"../resource/ToolStageBack/ToolBackImage.bmp", 2048, 2048);
	CCameraManager::GetInstance()->SetBackImageSize(10192, 2048);


	//SpawnMonster();

}

void CStageTutorial::Update(float fTime)
{

	CObjManager::GetInstance()->Update(fTime);

}

void CStageTutorial::LateUpdate(float fTime)
{
	CObjManager::GetInstance()->LateUpdate(fTime);

}

void CStageTutorial::Render(HDC mDC)
{
	CObjManager::GetInstance()->Render(mDC);

}

void CStageTutorial::Free()
{
	CObjManager::GetInstance()->Free();
}

void CStageTutorial::SpawnCamera()
{
	CObject* Camera = new CStageCamera{};

	CObjManager::GetInstance()->GetAllVector()[CObjManager::OBJECT_CAMERA].push_back(Camera);
}

void CStageTutorial::SpawnTile()
{
	ifstream ReadSteam("../SaveFile/LevelTutorial.txt");
	if (ReadSteam.fail())
	{
		ReadSteam.close();
		return;
	}

	// 기존 타일 삭제
	if (CObjManager::GetInstance()->GetAllVector()[CObjManager::OBJECT_TILE].size())
	{
		for (int j = 0; j < CObjManager::GetInstance()->GetAllVector()[CObjManager::OBJECT_TILE].size(); ++j)
		{
			delete CObjManager::GetInstance()->GetAllVector()[CObjManager::OBJECT_TILE][j];
			CObjManager::GetInstance()->GetAllVector()[CObjManager::OBJECT_TILE][j] = nullptr;
		}
		CObjManager::GetInstance()->GetAllVector()[CObjManager::OBJECT_TILE].clear();
	}


	if (CObjManager::GetInstance()->GetAllVector()[CObjManager::OBJECT_TILE2].size())
	{
		for (int j = 0; j < CObjManager::GetInstance()->GetAllVector()[CObjManager::OBJECT_TILE2].size(); ++j)
		{
			delete CObjManager::GetInstance()->GetAllVector()[CObjManager::OBJECT_TILE2][j];
			CObjManager::GetInstance()->GetAllVector()[CObjManager::OBJECT_TILE2][j] = nullptr;
		}
		CObjManager::GetInstance()->GetAllVector()[CObjManager::OBJECT_TILE2].clear();
	}

	//====================

	string InString{};

	POINT InCenterPos{};
	RECT INPOSRECT{};
	int INTILEType{};
	getline(ReadSteam, InString);
	while (true)
	{

		getline(ReadSteam, InString);

		stringstream iss(InString);

		iss >> InCenterPos.x >> InCenterPos.y >> INPOSRECT.left >> INPOSRECT.top >> INPOSRECT.right >> INPOSRECT.bottom >> INTILEType;

		CObject* NewTile = new CTile{};

		NewTile->SetCenterPos(InCenterPos);
		NewTile->SetPosRect(INPOSRECT);
		dynamic_cast<CTile*>(NewTile)->SetTileType(INTILEType);
		if (INTILEType >= CTile::TILE_POTAL_NOMAL_FRONT && INTILEType <= CTile::TILE_POTAL_LEFT_RIGHT_CHANGE_BACK)
		{
			CObjManager::GetInstance()->GetAllVector()[CObjManager::OBJECT_TILE].push_back(NewTile);

			NewTile = new CTile{};

			NewTile->SetCenterPos(InCenterPos);
			NewTile->SetPosRect(INPOSRECT);
			dynamic_cast<CTile*>(NewTile)->SetTileType(++INTILEType);
			CObjManager::GetInstance()->GetAllVector()[CObjManager::OBJECT_TILE2].push_back(NewTile);

		}
		else
		{
			CObjManager::GetInstance()->GetAllVector()[CObjManager::OBJECT_TILE].push_back(NewTile);
		}

		if (ReadSteam.eof())
		{
			break;
		}
	}

	ReadSteam.close();
}

void CStageTutorial::SpawnPlayer()
{
	CObject* pPlayer = new CPlayer{};
	CObjManager::GetInstance()->GetAllVector()[CObjManager::OBJECT_PLAYER].push_back(pPlayer);
	POINT Pos{ 200,400 };
	pPlayer->SetCenterPos(Pos);
	pPlayer->SetRidius(25.f);
}

void CStageTutorial::CreatEscMenu()
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