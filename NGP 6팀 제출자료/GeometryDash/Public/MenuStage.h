#pragma once
#include "Stage.h"

class CMenuStage : public CStage
{
public:
	CMenuStage();
	~CMenuStage();

public:

	// CStage을(를) 통해 상속됨
	void Initailizer() override;

	void Update(float fTime) override;

	void LateUpdate(float fTime) override;

	void Render(HDC mDC) override;

	void Free() override;

public:

	void SpawnLand();

private:
	vector<HBITMAP*> m_vechBitMap{};

	HDC m_APlusDC{};

};

