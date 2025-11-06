#pragma once
#include "Object.h"


class CTile : public CObject
{
public:
	CTile();
	~CTile();

public:
	enum TileType {TILE_STONE,TILE_SPLINTER_UP,
		TILE_SPLINTER_LEFT, TILE_SPLINTER_RIGHT, TILE_SPLINTER_DOWN,
		TILE_POTAL_NOMAL_FRONT, TILE_POTAL_NOMAL_BACK, 
		TILE_POTAL_AIR_PLANE_FRONT, TILE_POTAL_AIR_PLANE_BACK,
		TILE_POTAL_ZIGZAG_FRONT, TILE_POTAL_ZIGZAG_BACK,
		TILE_POTAL_NOMAL_LEFT_RIGHT_FRONT, TILE_POTAL_NOMAL_LEFT_RIGHT_BACK,
		TILE_POTAL_LEFT_RIGHT_CHANGE_FRONT, TILE_POTAL_LEFT_RIGHT_CHANGE_BACK,
		TILE_JUMP_FLOOR, TILE_JUMP_CIRCLE,
		TILE_END};


	// CObject을(를) 통해 상속됨
	void Initailizer() override;

	int Update(float fTime) override;

	void LateUpdate(float fTime) override;

	void Render(HDC mDC) override;

	void Free() override;

	

public:
	void SetTileType(int _TileType)
	{
		m_eTileType = (TileType)_TileType;
	}

	int GetTileType()
	{
		return m_eTileType;
	}


	void Collision();

//private:



private:
	TileType m_eTileType{ TILE_STONE };

	vector<HBITMAP*> m_vechBitMap{};
	HDC m_TileDc{};



	bool m_bPotalCollosion{}; // 포탈 전용 충돌 했는지 안했는지?

};

