#include "ObjManager.h"
#include <algorithm>

CObjManager* CObjManager::m_pInstance = nullptr;

CObjManager::CObjManager()
	: m_PlayerID(-1) // 초기값을 -1로 설정 (미할당 상태)
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
	// 플레이어 ID 리스트 정리
	m_OtherPlayerIDs.clear();
	m_PlayerID = -1; // ID 초기화
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

// --- Other Player ID 관리 함수 구현 ---

bool CObjManager::AddOtherPlayerID(int id)
{
	// 로컬 플레이어 ID와 중복 불가
	if (id == m_PlayerID)
		return false;

	// 이미 존재하면 추가 안 함
	if (std::find(m_OtherPlayerIDs.begin(), m_OtherPlayerIDs.end(), id) != m_OtherPlayerIDs.end())
		return false;

	m_OtherPlayerIDs.push_back(id);
	return true;
}

bool CObjManager::RemoveOtherPlayerID(int id)
{
	auto it = std::find(m_OtherPlayerIDs.begin(), m_OtherPlayerIDs.end(), id);
	if (it == m_OtherPlayerIDs.end())
		return false;

	m_OtherPlayerIDs.erase(it);
	return true;
}

int CObjManager::GetOtherPlayerID(int index) const
{
	if (index < 0 || index >= (int)m_OtherPlayerIDs.size())
		return -1;
	return m_OtherPlayerIDs[index];
}

bool CObjManager::IsOtherPlayer(int id) const
{
	return std::find(m_OtherPlayerIDs.begin(), m_OtherPlayerIDs.end(), id) != m_OtherPlayerIDs.end();
}

void CObjManager::ClearOtherPlayers()
{
	m_OtherPlayerIDs.clear();
}