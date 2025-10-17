#pragma once
#include "Stage.h"

class CToolStage : public CStage
{
public:
	CToolStage();
	~CToolStage();

public:

	// CStage을(를) 통해 상속됨
	void Initailizer() override;

	void Update(float fTime) override;

	void LateUpdate(float fTime) override;

	void Render(HDC mDC) override;

	void Free() override;

public:
	void PrintGrid(HDC mDC);

	void CreatCamera();
	void CreatMouse();

	void CreatEscMenu();

private:
	float iGridSizeX{TileSizeX};
	float iGridSizeY{ TileSizeY};

	float iBackSizeX{150.f};
	float iBackSizeY{1500.f};
};

