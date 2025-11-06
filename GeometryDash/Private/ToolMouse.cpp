#include "ToolMouse.h"
#include "CameraManager.h"
#include "KeyManager.h"
#include "ObjManager.h"
#include "Tile.h"

#include <fstream>
#include <sstream>
#include <string>

CToolMouse::CToolMouse()
{
    Initailizer();
}

CToolMouse::~CToolMouse()
{
    Free();
}

void CToolMouse::Initailizer()
{
	
	HBITMAP* InBitMap{};
	InBitMap = new HBITMAP;
	*InBitMap = (HBITMAP)LoadImage(g_hInst, L"../resource/ToolMouse/ToolMouse.bmp", IMAGE_BITMAP, 512, 512, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
	m_vechBitMap.push_back(InBitMap);
	m_fRidius = 10;
	TCHAR Name[50] = {};
	for (int i = 1; i <= AllTileNum; ++i)
	{
		wsprintf(Name, L"../resource/Object/툴 사용타일/타일%d.bmp", i);
		InBitMap = new HBITMAP;
		*InBitMap = (HBITMAP)LoadImage(g_hInst, Name, IMAGE_BITMAP, 128, 128, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		m_vechBitMap.push_back(InBitMap);
	}
}

int CToolMouse::Update(float fTime)
{
	//ShowCursor(false);
	KeyDown();
    GetCursorPos(&m_CenterPos);
    ScreenToClient(g_hWnd, &m_CenterPos);

	m_MouseGridPos.x = (int)((m_CenterPos.x + CCameraManager::GetInstance()->GetCenterPos().x) / TileSizeX) * TileSizeX;
	m_MouseGridPos.y = (int)((m_CenterPos.y + CCameraManager::GetInstance()->GetCenterPos().y) / TileSizeY) * TileSizeY;

	Collosition();
    return 0;
}

void CToolMouse::LateUpdate(float fTime)
{
	

}

void CToolMouse::Render(HDC mDC)
{
	m_MouseDc = CreateCompatibleDC(mDC);
	//
	HPEN hPen = CreatePen(PS_SOLID, 2, RGB(255, 255, 255)), OldPen;
	OldPen = (HPEN)SelectObject(mDC, hPen);

	HBRUSH hBrush = (HBRUSH)GetStockObject(NULL_BRUSH), OldBrush;
	OldBrush = (HBRUSH)SelectObject(mDC, hBrush);

	Rectangle(mDC, m_MouseGridPos.x - CCameraManager::GetInstance()->GetCenterPos().x, m_MouseGridPos.y - CCameraManager::GetInstance()->GetCenterPos().y,
		m_MouseGridPos.x + TileSizeX - CCameraManager::GetInstance()->GetCenterPos().x, m_MouseGridPos.y + TileSizeY - CCameraManager::GetInstance()->GetCenterPos().y);


	m_TileDC = CreateCompatibleDC(mDC);

	SelectObject(m_TileDC, *(m_vechBitMap[m_eNowSelectTile+1]));

	TransparentBlt(mDC, m_CenterPos.x,
		m_CenterPos.y,
		m_fRidius * 4.f, m_fRidius * 4.f, m_TileDC, 0, 0, 128, 128, RGB(255, 0, 255));

	DeleteObject(m_TileDC);

	SelectObject(mDC, OldBrush);
	SelectObject(mDC, OldPen);
	DeleteObject(hPen);
	DeleteObject(hBrush);
	//
	SelectObject(m_MouseDc, *(m_vechBitMap[0]));

	//TransparentBlt(mDC, m_CenterPos.x,
		//m_CenterPos.y,
		//m_fRidius * 2.f, m_fRidius * 2.f, m_MouseDc, 0, 0, 128, 128, RGB(255, 0, 255));
	
	DeleteObject(m_MouseDc);
}

void CToolMouse::Free()
{
	ShowCursor(true);
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
}

void CToolMouse::KeyDown()
{
	if (CKeyManager::GetInstance()->KeyDown('Q'))
	{
		if (m_eNowSelectTile >= CTile::TILE_POTAL_NOMAL_FRONT && m_eNowSelectTile <= CTile::TILE_POTAL_LEFT_RIGHT_CHANGE_FRONT)
		{
			m_eNowSelectTile = m_eNowSelectTile + 2;
		}
		else
		{
			m_eNowSelectTile++;
		}
		if (m_eNowSelectTile == CTile::TILE_END)
		{
			m_eNowSelectTile = CTile::TILE_STONE;
		}
	}







	// 저장
	if (CKeyManager::GetInstance()->KeyDown('B'))
	{
		SaveAllTile();
	}

	if (CKeyManager::GetInstance()->KeyDown('N'))
	{
		LoadFile();
	}
	
}

void CToolMouse::Collosition()
{

	if (CKeyManager::GetInstance()->KeyPressing(VK_LBUTTON))
	{
		if (CObjManager::GetInstance()->GetAllVector()[CObjManager::OBJECT_TILE].size())
		{
			for (auto iter = CObjManager::GetInstance()->GetAllVector()[CObjManager::OBJECT_TILE].begin();
				iter != CObjManager::GetInstance()->GetAllVector()[CObjManager::OBJECT_TILE].end();
				++iter)
			{
				RECT CheckRect = (*iter)->GetPosRect();
				if (PtInRect(&CheckRect, m_MouseGridPos)) // 만약 현재 클릭한 위치에 어떠한 오브젝트가 있다면?
				{
					return; //생서하지 않고 나가기
				}
			}
		}
		if (m_eNowSelectTile >= CTile::TILE_POTAL_NOMAL_FRONT && m_eNowSelectTile <= CTile::TILE_POTAL_LEFT_RIGHT_CHANGE_BACK)
		{
			CObject* NewTile = new CTile{};
			POINT InPoint{ m_MouseGridPos.x + TileSizeX * 0.5f,m_MouseGridPos.y + TileSizeY * 0.5f };
			dynamic_cast<CTile*>(NewTile)->SetCenterPos(InPoint);
			dynamic_cast<CTile*>(NewTile)->SetTileType((int)((m_eNowSelectTile/2)* 2)+ (m_eNowSelectTile % 2));
			CObjManager::GetInstance()->GetAllVector()[CObjManager::OBJECT_TILE].push_back(NewTile);

		}
		else
		{
			CObject* NewTile = new CTile{};
			POINT InPoint{ m_MouseGridPos.x + TileSizeX * 0.5f,m_MouseGridPos.y + TileSizeY * 0.5f };
			dynamic_cast<CTile*>(NewTile)->SetCenterPos(InPoint);
			dynamic_cast<CTile*>(NewTile)->SetTileType(m_eNowSelectTile);
			CObjManager::GetInstance()->GetAllVector()[CObjManager::OBJECT_TILE].push_back(NewTile);
		}
	}

	if (CKeyManager::GetInstance()->KeyPressing(VK_RBUTTON))
	{
		if (CObjManager::GetInstance()->GetAllVector()[CObjManager::OBJECT_TILE].size())
		{
			for (auto iter = CObjManager::GetInstance()->GetAllVector()[CObjManager::OBJECT_TILE].begin();
				iter != CObjManager::GetInstance()->GetAllVector()[CObjManager::OBJECT_TILE].end();
				++iter)
			{
				RECT CheckRect = (*iter)->GetPosRect();
				if (PtInRect(&CheckRect, m_MouseGridPos)) // 만약 현재 클릭한 위치에 어떠한 오브젝트가 있다면?
				{
				
					delete  (*iter);
					(*iter) = nullptr;
					iter = CObjManager::GetInstance()->GetAllVector()[CObjManager::OBJECT_TILE].erase(iter);
					break;
				}
			}
		}
	}
}

void CToolMouse::SaveAllTile()
{
	ofstream inStream("../SaveFile/EditSave.txt");
	if (inStream.fail())
	{
		inStream.close();
		return;
	}
	if (CObjManager::GetInstance()->GetAllVector()[CObjManager::OBJECT_TILE].size() == 0)
	{
		return;
	}

	string Instring{};
	for (auto iter = CObjManager::GetInstance()->GetAllVector()[CObjManager::OBJECT_TILE].begin();
		iter != CObjManager::GetInstance()->GetAllVector()[CObjManager::OBJECT_TILE].end();
		++iter)
	{
		//stringstream iss(Instring);
		long INCenterPOSX = (*iter)->GetCenterPos().x;
		long INCenterPOSY = (*iter)->GetCenterPos().y;
		long INPOSRECTL = (*iter)->GetPosRect().left;
		long INPOSRECTT = (*iter)->GetPosRect().top;
		long INPOSRECTR = (*iter)->GetPosRect().right;
		long INPOSRECTB = (*iter)->GetPosRect().bottom;
		int INTILEType = dynamic_cast<CTile*>(*iter)->GetTileType();
		if (INTILEType >= CTile::TILE_POTAL_NOMAL_FRONT && INTILEType <= CTile::TILE_POTAL_LEFT_RIGHT_CHANGE_BACK)
		{
			if (INTILEType % 2 == 0) // back
			{
				INTILEType = (int)((INTILEType) / 2) * 2-1;
			}
			else // front
			{
				INTILEType = (int)((INTILEType) / 2) * 2 +1 ;
			}
		}
			inStream << '\n' << INCenterPOSX << " " << INCenterPOSY << " " << INPOSRECTL << " " << INPOSRECTT << " " << INPOSRECTR << " "
				<< INPOSRECTB << " " << INTILEType;
	}

	inStream.close();
}

void CToolMouse::LoadFile()
{
	ifstream ReadSteam("../SaveFile/EditSave.txt");
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
	}
	CObjManager::GetInstance()->GetAllVector()[CObjManager::OBJECT_TILE].clear();

	if (CObjManager::GetInstance()->GetAllVector()[CObjManager::OBJECT_TILE2].size())
	{
		for (int j = 0; j < CObjManager::GetInstance()->GetAllVector()[CObjManager::OBJECT_TILE2].size(); ++j)
		{
			delete CObjManager::GetInstance()->GetAllVector()[CObjManager::OBJECT_TILE2][j];
			CObjManager::GetInstance()->GetAllVector()[CObjManager::OBJECT_TILE2][j] = nullptr;
		}
	}
	CObjManager::GetInstance()->GetAllVector()[CObjManager::OBJECT_TILE2].clear();
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

		iss >> InCenterPos.x >> InCenterPos.y>> INPOSRECT.left>> INPOSRECT.top>> INPOSRECT.right>> INPOSRECT.bottom >> INTILEType;

		CObject* NewTile = new CTile{};

		NewTile->SetCenterPos(InCenterPos);
		NewTile->SetPosRect(INPOSRECT);
		dynamic_cast<CTile*>(NewTile)->SetTileType(INTILEType);
		if (INTILEType >= CTile::TILE_POTAL_NOMAL_FRONT && INTILEType <= CTile::TILE_POTAL_LEFT_RIGHT_CHANGE_BACK)
		{
			CObjManager::GetInstance()->GetAllVector()[CObjManager::OBJECT_TILE].push_back(NewTile);
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
