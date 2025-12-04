#pragma once
#include "Object.h"

class CCollisionManager : public CObject
{
public:
	CCollisionManager();
	virtual ~CCollisionManager();

public:

	// CObject을(를) 통해 상속됨
	void Initailizer() override;

	int Update(float fTime) override;

	void LateUpdate(float fTime) override;

	void Render(HDC mDC) override;

	void Free() override;
	
public:
	bool CheckCircle(CObject* _Dst, CObject* _Src); // 구 충돌 함수

	bool CheckCircle_And_Point(CObject* _Dst, POINT _Src); // 구와 점 충돌 함수
	
	bool CheckRect(CObject* _Dst, CObject* _Src); // 네모충돌

public:
	static CCollisionManager* GetInstance()
	{
		if (m_pInstance == nullptr)
		{
			m_pInstance = new CCollisionManager{};
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

private:
	static CCollisionManager* m_pInstance;

};

