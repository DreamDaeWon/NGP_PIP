#pragma once
#include "Object.h"

class CCollisionManager : public CObject
{
public:
	CCollisionManager();
	virtual ~CCollisionManager();

public:

	// CObject��(��) ���� ��ӵ�
	void Initailizer() override;

	int Update(float fTime) override;

	void LateUpdate(float fTime) override;

	void Render(HDC mDC) override;

	void Free() override;
	
public:
	bool CheckCircle(CObject* _Dst, CObject* _Src); // �� �浹 �Լ�

	bool CheckCircle_And_Point(CObject* _Dst, POINT _Src); // ���� �� �浹 �Լ�
	
	bool CheckRect(CObject* _Dst, CObject* _Src); // �׸��浹

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

