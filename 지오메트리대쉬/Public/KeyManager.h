#pragma once
#include "Object.h"

class CKeyManager : public CObject
{
public:
	CKeyManager();
	~CKeyManager();
public:

	// CObject��(��) ���� ��ӵ�
	void Initailizer() override;

	int Update(float fTime) override;

	void LateUpdate(float fTime) override;

	void Render(HDC mDC) override;

	void Free() override;


public:
	// ���� ��
	bool KeyDown(int _iKey);

	// �����ٰ� �� ��
	bool KeyUp(int _iKey);

	// ������ �ִ� ���� ��
	bool KeyPressing(int _iKey);

	void ResetAllKey();
public:

	void UpdatebKey(int _iKey);

	void UpdateBeforeKey();


public:
	static CKeyManager* GetInstance();

	static void DestoryInstance()
	{
		delete m_pKeyManager;
		m_pKeyManager = nullptr;
	}

private:
	bool m_bKey[VK_MAX]{};
	bool m_bBeforeKey[VK_MAX]{};



private:
	static CKeyManager* m_pKeyManager;

};

