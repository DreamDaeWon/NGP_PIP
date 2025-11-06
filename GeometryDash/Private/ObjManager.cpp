#include "ObjManager.h"

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
