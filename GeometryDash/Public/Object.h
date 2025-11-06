#pragma once

#include <Windows.h>
#include "Default.h"

class CPlayer;

class CObject
{
public:
	CObject();
	virtual ~CObject();


public:
	virtual void Initailizer() = 0;

	virtual int Update(float fTime) = 0;

	virtual void LateUpdate(float fTime) = 0;

	virtual void Render(HDC mDC) = 0;

	virtual void Free() = 0;


	void SetPosRect(RECT _Rc) { m_POSRECT = _Rc; }

	RECT GetPosRect() { return m_POSRECT; }

	void SetCenterPos(POINT _Pos) { m_CenterPos = _Pos; }


	POINT GetCenterPos() { return m_CenterPos; }

	void ResetPosRect() { m_POSRECT = RECT{ (long)(m_CenterPos.x - m_fRidius), (long)(m_CenterPos.y - m_fRidius), (long)(m_CenterPos.x + m_fRidius), (long)(m_CenterPos.y + m_fRidius) }; }

	void SetRidius(float _Size) { m_fRidius = _Size; }

	float GetRidius() { return m_fRidius; }

	void SetDead() { m_bDead = true; }

protected:
	bool m_bDead{};
	RECT m_POSRECT{};
	POINT m_CenterPos{};
	float m_fRidius{};
};

