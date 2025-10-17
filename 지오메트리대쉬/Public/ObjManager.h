#pragma once
#include "Object.h"

class CObjManager : public CObject
{

public:
	CObjManager();
	virtual ~CObjManager();

public:
	// CObject을(를) 통해 상속됨
	void Initailizer() override;

	int Update(float fTime) override;

	void LateUpdate(float fTime) override;

	void Render(HDC mDC) override;

	void Free() override;


public:
	static CObjManager* GetInstance()
	{
		if (m_pInstance == nullptr)
		{
			m_pInstance = new CObjManager{};
			return m_pInstance;
		}
		else
			return m_pInstance;
	}

	static void DestoryInstance()
	{
		delete m_pInstance;
		m_pInstance = nullptr;
	}

	enum ObjectType { OBJECT_BACK, OBJECT_CAMERA,OBJECT_WALL,OBJECT_TILE, OBJECT_ITEM, OBJECT_MONSTER, 
		OBJECT_BULLET, OBJECT_PLAYER, OBJECT_TILE2, OBJECT_EFFECT,OBECT_ESC_MENU, OBJECT_UI, OBJECT_MOUSE, OBJECT_LAND, OBJECT_BUTTON,OBJECT_END };

	vector<CObject*>* GetAllVector() { return vecAllObj; }
	
	void DeleteVector(ObjectType _Type);


private:
	static CObjManager* m_pInstance;
	vector<CObject*>vecAllObj[OBJECT_END]{};

};

