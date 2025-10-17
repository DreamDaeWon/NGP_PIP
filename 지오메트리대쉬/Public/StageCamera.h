#pragma once
#include "Object.h"

class CStageCamera : public CObject
{
public:
	CStageCamera();
	~CStageCamera();

public:

	// CObject을(를) 통해 상속됨
	void Initailizer() override;

	int Update(float fTime) override;

	void LateUpdate(float fTime) override;

	void Render(HDC mDC) override;

	void Free() override;


private:
	float m_fSpeed{ 10.f };


};

