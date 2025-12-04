#pragma once

#include "Object.h"
#include "math.h"


#define PI 3.141592

class CLand : public CObject
{
public:
	CLand();

	~CLand();


	enum DIR { DIR_UP, DIR_DOWN, DIR_END };

public:
	// CObject을(를) 통해 상속됨
	void Initailizer() override;

	int Update(float fTime) override;

	void LateUpdate(float fTime) override;

	void Render(HDC mDC) override;

	void Free() override;

public:
	void Collision();

	float radian(float degrees);

private:
	vector<HBITMAP*> m_vechBitMap{};
	HDC m_LandDc{};

	float m_fTime{};

	float m_fSpeed{ 10.f };

	float angle{};


	float fY{};

	DIR m_NowDir{ DIR_UP };
};

