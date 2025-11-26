#include "ObjManager.h"
#include "Player.h"
#include "../DaeWonFrameWork/OtherPlayer.h"
#include <algorithm>

CObjManager* CObjManager::m_pInstance = nullptr;

CObjManager::CObjManager()
{
}

CObjManager::~CObjManager()
{
	Free();
}

void CObjManager::Initailizer()
{

}

int CObjManager::Update(float fTime)
{
	for (int i = 0; i < OBJECT_END; ++i)
	{
		if (vecAllObj[i].size())
		{
			for (int j = 0; j < vecAllObj[i].size(); ++j)
			{
				if (vecAllObj[i][j]->Update(fTime) == -1)
				{
					delete vecAllObj[i][j];
					vecAllObj[i][j] = nullptr;

					vecAllObj[i].erase(vecAllObj[i].begin() + j);
				}
			}
		}
	}
	return 0;
}

void CObjManager::LateUpdate(float fTime)
{
	for (int i = 0; i < OBJECT_END; ++i)
	{
		if (vecAllObj[i].size())
		{
			for (int j = 0; j < vecAllObj[i].size(); ++j)
			{
				vecAllObj[i][j]->LateUpdate(fTime);
			}
		}
	}
}

void CObjManager::Render(HDC mDC)
{
	for (int i = 0; i < OBJECT_END; ++i)
	{
		if (vecAllObj[i].size())
		{
			for (int j = 0; j < vecAllObj[i].size(); ++j)
			{
				vecAllObj[i][j]->Render(mDC);
			}
		}
	}
}

void CObjManager::Free()
{
	for (int i = 0; i < OBJECT_END; ++i)
	{
		if (vecAllObj[i].size())
		{
			for (int j = 0; j < vecAllObj[i].size(); ++j)
			{
				delete vecAllObj[i][j];
				vecAllObj[i][j] = nullptr;
			}
		}
		vecAllObj[i].clear();
	}
}

void CObjManager::Free_But_OtherPlayer_Exclude()
{
	for (int i = 0; i < OBJECT_END; ++i)
	{
		if(i != OBJECT_OTHERPLAYER)
		{
			if (vecAllObj[i].size())
			{
				for (int j = 0; j < vecAllObj[i].size(); ++j)
				{
					delete vecAllObj[i][j];
					vecAllObj[i][j] = nullptr;
				}
			}
			vecAllObj[i].clear();
		}
	}
}

void CObjManager::DeleteVector(ObjectType _Type)
{
	if (vecAllObj[_Type].size())
	{
		for (int j = 0; j < vecAllObj[_Type].size(); ++j)
		{
			delete vecAllObj[_Type][j];
			vecAllObj[_Type][j] = nullptr;
		}
	}
	vecAllObj[_Type].clear();
}

void CObjManager::SetMyPlayer(CPlayer* _player)
{
	_MyPlayer = _player;
}

void CObjManager::SetMyPlayerID(int id)
{
	if (_MyPlayer)
		_MyPlayer->SetID(id);
}

int CObjManager::GetMyPlayerID() const 
{
	if (_MyPlayer)
		return _MyPlayer->GetID();
	return -1; 
}

// --- 다른 플레이어 관련 ---

COtherPlayer* CObjManager::FindOtherPlayer(int id)
{
	for (CObject* player : vecAllObj[OBJECT_OTHERPLAYER])
	{
		COtherPlayer* otherPlayer = dynamic_cast<COtherPlayer*>(player);
		if (otherPlayer->GetID() == id)
			return otherPlayer;
	}
	return nullptr;
}

void CObjManager::AddOtherPlayer(int id, float x, float y)
{
	// 1. 이미 있는 플레이어인지 확인 (중복 생성 방지)
	if (FindOtherPlayer(id) != nullptr) return;

	// 2. 내 ID와 같은지 확인 (나는 OtherPlayer로 만들면 안 됨)
	if (id == GetMyPlayerID()) return;

	// 3. 생성 및 초기화
	COtherPlayer* newPlayer = new COtherPlayer();
	newPlayer->SetID(id);

	// 4. 리스트에 추가
	vecAllObj[OBJECT_OTHERPLAYER].emplace_back(newPlayer);
}

void CObjManager::RemoveOtherPlayer(int id)
{
	auto& vec = vecAllObj[OBJECT_OTHERPLAYER];
	for (auto iter = vec.begin(); iter != vec.end(); ++iter)
	{
		COtherPlayer* pOther = dynamic_cast<COtherPlayer*>(*iter);
		if (pOther && pOther->GetID() == id)
		{
			delete* iter;   // 메모리 해제
			vec.erase(iter); // 벡터에서 제거
			return;
		}
	}
}

void CObjManager::ClearOtherPlayers()
{
	auto& vec = vecAllObj[OBJECT_OTHERPLAYER];
	for (auto pObj : vec)
	{
		delete pObj;
	}
	vec.clear();
}