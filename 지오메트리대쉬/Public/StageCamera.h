#pragma once
#include "Object.h"

class CStageCamera : public CObject
{
public:
	CStageCamera();
	~CStageCamera();

public:

	// CObject��(��) ���� ��ӵ�
	void Initailizer() override;

	int Update(float fTime) override;

	void LateUpdate(float fTime) override;

	void Render(HDC mDC) override;

	void Free() override;


private:
	float m_fSpeed{ 10.f };


};

