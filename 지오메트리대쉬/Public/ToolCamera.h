#pragma once
#include "Object.h"

class CToolCamera : public CObject
{
public:
	CToolCamera();
	~CToolCamera();

public:

	// CObject��(��) ���� ��ӵ�
	void Initailizer() override;

	int Update(float fTime) override;

	void LateUpdate(float fTime) override;

	void Render(HDC mDC) override;

	void Free() override;


private:
	float m_fSpeed{10.f};


};

