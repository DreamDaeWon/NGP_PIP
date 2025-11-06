#pragma once
#include "Object.h"

#define PI 3.141592

class CGoTutorialStageButton : public CObject
{
public:
	CGoTutorialStageButton();

	~CGoTutorialStageButton();
public:
	// CObject을(를) 통해 상속됨
	void Initailizer() override;

	int Update(float fTime) override;

	void LateUpdate(float fTime) override;

	void Render(HDC mDC) override;

	void Free() override;

public:

	float radian(float degrees);

private:
	vector<HBITMAP*> m_vechBitMap{};
	HDC m_ButtonDc{};

	float m_fTime{};

	float m_fSpeed{ 10.f };

	float angle{};


	float fY{};
};

