#pragma once
#include "Object.h"

class CGoBackButton : public CObject
{
public:
	CGoBackButton();
	~CGoBackButton();


public:
	// CObject을(를) 통해 상속됨
	void Initailizer() override;
	int Update(float fTime) override;
	void LateUpdate(float fTime) override;
	void Render(HDC mDC) override;
	void Free() override;

public:
	void LoadButtonSound();
	void SaveAllTile();

private:
	vector<HBITMAP*> m_vechBitMap{};
	HDC m_ButtonDc{};

};


