#pragma once
#include "Default.h"
#include "Stage.h"
using namespace std;

class CMainGame
{
public:
	CMainGame();
	~CMainGame();

public:

	void Initailizer();

	void Update(float fTime);

	void LateUpdate(float fTime);

	void Render(HDC mDC);

	void Free();

private:

	bool m_bStart{};

};

