#pragma once
#include "Default.h"

class CStage
{
public:
	CStage();
	virtual ~CStage();

public:
	virtual void Initailizer() = 0;

	virtual void Update(float fTime) = 0;

	virtual void LateUpdate(float fTime) = 0;

	virtual void Render(HDC mDC) = 0;

	virtual void Free() = 0;
};

