#pragma once
#include "Object.h"

class CGoToolButton : public CObject
{
public:
	CGoToolButton();
	~CGoToolButton();


public:
	// CObject을(를) 통해 상속됨
	void Initailizer() override;
	int Update(float fTime) override;
	void LateUpdate(float fTime) override;
	void Render(HDC mDC) override;
	void Free() override;

public:
	void LoadButtonSound();


private:
	vector<HBITMAP*> m_vechBitMap{};
	HDC m_ButtonDc{};

};

