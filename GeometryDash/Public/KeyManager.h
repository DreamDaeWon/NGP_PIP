#pragma once
#include "Object.h"

class CKeyManager : public CObject
{
public:
	CKeyManager();
	~CKeyManager();
public:

	// CObject을(를) 통해 상속됨
	void Initailizer() override;

	int Update(float fTime) override;

	void LateUpdate(float fTime) override;

	void Render(HDC mDC) override;

	void Free() override;


public:
	// 누를 때
	bool KeyDown(int _iKey);

	// 눌렀다가 뗄 때
	bool KeyUp(int _iKey);

	// 누르고 있는 중일 때
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

