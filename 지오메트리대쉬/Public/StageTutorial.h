#pragma once
#include "Stage.h"
#include "Object.h"

class CStageTutorial : public CStage
{
public:
	CStageTutorial();
	~CStageTutorial();

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

