#pragma once
#include "Object.h"

class CGoStageButton : public CObject
{
public:
	CGoStageButton();
	~CGoStageButton();

public:

	// CObject을(를) 통해 상속됨
	void Initailizer() override;

	int Update(float fTime) override;

	void LateUpdate(float fTime) override;

	void Render(HDC mDC) override;

	void Free() override;

private:
	vector<HBITMAP*> m_vechBitMap{};
	HDC m_ButtonDc{};

	HDC m_NameDc{};

	HDC m_MadeByDc{};

};
