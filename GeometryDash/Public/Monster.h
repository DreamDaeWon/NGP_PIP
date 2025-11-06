#pragma once
#include "Object.h"

class CMonster : public CObject
{
public:
	CMonster();
	~CMonster() override;

	enum DIR { DIR_LEFT, DIR_RIGHT, DIR_UP, DIR_DOWN, DIR_SPACE, DIR_END };

public:

	void Initailizer() override;

	int Update(float fTime) override;

	void LateUpdate(float fTime) override;

	void Render(HDC mDC) override;

	void Free() override;

public:

	void Collision();

	void DownHp() { --m_iHp; }

private:
	enum Resource {RS_IDLE, RS_DEATH,RS_END};

	Resource m_NowResource{};
	
	vector<HBITMAP*> m_vechBitMap[RS_END]{};
	HDC m_PalyerDc{};

	float m_fTime{};

	bool Key[DIR_END]{};

	float m_fSpeed{ 300.f };

	bool m_bDeath{}; // Á×¾ú´ÂÁö?

	int m_iHp{ 5 };

	DIR m_NowDir{};
};

