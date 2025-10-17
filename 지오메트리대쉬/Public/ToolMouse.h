#pragma once
#include "Object.h"

class CToolMouse : public CObject
{
public:
	CToolMouse();
	virtual ~CToolMouse();

public:

	// CObject��(��) ���� ��ӵ�
	void Initailizer() override;

	int Update(float fTime) override;

	void LateUpdate(float fTime) override;

	void Render(HDC mDC) override;

	void Free() override;



public:
	void KeyDown();

	void Collosition();


public: // ���� �����

	void SaveAllTile();

	void LoadFile();


private:
	vector<HBITMAP*> m_vechBitMap{};

	HDC m_MouseDc{};

	HDC m_TileDC{};

	POINT m_MouseGridPos{}; // ���콺 �����ӿ� ���� �׸��� �߾� ��

	int m_eNowSelectTile{}; // ���� ���� Ÿ�� enumŸ�� ��ȣ
	
	bool m_bEraserMode{false}; // ���찳 �������?

	bool m_bKeyControl = { false };

};

