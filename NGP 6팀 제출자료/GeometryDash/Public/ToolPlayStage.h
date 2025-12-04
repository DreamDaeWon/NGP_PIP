#pragma once
#include "Stage.h"
#include "Object.h"

class CToolPlayStage : public CStage
{
public:
	CToolPlayStage();
	~CToolPlayStage();

public:

	void Initailizer() override;

	void Update(float fTime) override;

	void LateUpdate(float fTime) override;

	void Render(HDC mDC) override;

	void Free() override;


public:
	void SpawnCamera();


	void SpawnTile();

	void SpawnPlayer();

	void CreatEscMenu();

private:

	bool bEscMenu{};

};

