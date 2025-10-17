#pragma once
#include "Object.h"

class CToolMouse : public CObject
{
public:
	CToolMouse();
	virtual ~CToolMouse();

public:

	// CObject을(를) 통해 상속됨
	void Initailizer() override;

	int Update(float fTime) override;

	void LateUpdate(float fTime) override;

	void Render(HDC mDC) override;

	void Free() override;



public:
	void KeyDown();

	void Collosition();


public: // 파일 입출력

	void SaveAllTile();

	void LoadFile();


private:
	vector<HBITMAP*> m_vechBitMap{};

	HDC m_MouseDc{};

	HDC m_TileDC{};

	POINT m_MouseGridPos{}; // 마우스 움직임에 따른 그리드 중앙 값

	int m_eNowSelectTile{}; // 현재 선택 타일 enum타입 번호
	
	bool m_bEraserMode{false}; // 지우개 모드인지?

	bool m_bKeyControl = { false };

};

