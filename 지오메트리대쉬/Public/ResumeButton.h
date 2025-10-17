#pragma once
#include "Object.h"

class CResumeButton : public CObject
{
public:
	CResumeButton();
	~CResumeButton();


public:
	// CObject��(��) ���� ��ӵ�
	void Initailizer() override;
	int Update(float fTime) override;
	void LateUpdate(float fTime) override;
	void Render(HDC mDC) override;
	void Free() override;

public:
	void LoadButtonSound();
	void SaveAllTile();

private:
	vector<HBITMAP*> m_vechBitMap{};
	HDC m_ButtonDc{};

};

