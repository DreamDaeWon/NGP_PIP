#pragma once
#include "Stage.h"
#include "Object.h"

class CStageOne : public CStage
{
public:
	CStageOne();
	~CStageOne();

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

public:
	
private:

	bool bEscMenu{};

};

