#pragma once

#include "Stage.h"



class CMainStage : public CStage
{
public:
	CMainStage();
	~CMainStage();

public:
	// CStage��(��) ���� ��ӵ�
	void Initailizer() override;

	void Update(float fTime) override;

	void LateUpdate(float fTime) override;

	void Render(HDC mDC) override;

	void Free() override;

public:
	void SpawnCamera();

	void SpawnButton();
};

