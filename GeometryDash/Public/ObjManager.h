#pragma once
#include "Object.h"
#include <vector>

class CPlayer;
class COtherPlayer;

class CObjManager : public CObject
{

public:
	CObjManager();
	virtual ~CObjManager();

public:
	// CObject의(를) 상속 받은 함수들
	void Initailizer() override;

	int Update(float fTime) override;

	void LateUpdate(float fTime) override;

	void Render(HDC mDC) override;

	void Free() override;

public:
	static CObjManager* GetInstance()
	{
		if (m_pInstance == nullptr)
		{
			m_pInstance = new CObjManager{};
			return m_pInstance;
		}
		else
			return m_pInstance;
	}

	static void DestoryInstance()
	{
		delete m_pInstance;
		m_pInstance = nullptr;
	}

	enum ObjectType {
		OBJECT_BACK, OBJECT_CAMERA, OBJECT_WALL, OBJECT_TILE, OBJECT_ITEM, OBJECT_MONSTER,
		OBJECT_BULLET, OBJECT_PLAYER, OBJECT_OTHERPLAYER, OBJECT_TILE2, OBJECT_EFFECT, OBECT_ESC_MENU, OBJECT_UI, OBJECT_MOUSE, OBJECT_LAND, OBJECT_BUTTON, OBJECT_END
	};

	std::vector<CObject*>* GetAllVector() { return vecAllObj; }

	void DeleteVector(ObjectType _Type);


private:
	static CObjManager* m_pInstance;
	std::vector<CObject*>vecAllObj[OBJECT_END]{};

	CPlayer* _MyPlayer;

public:
	void SetMyPlayer(CPlayer* _player);				 // 내 player 설정
	void SetMyPlayerID(int id);						 // 내 player ID 설정 (Login 패킷 송신 시 사용)
	int GetMyPlayerID() const;						 // 내 player ID 반환
	CPlayer* GetMyPlayer() const;					 // 내 player 포인터 반환

	COtherPlayer* FindOtherPlayer(int id);			 // ID로 다른 플레이어 찾기 
	void AddOtherPlayer(int id, float x, float y);   // 다른 플레이어 추가 (RoomStart 패킷 수신 시 삭제)
	void RemoveOtherPlayer(int id);					 // 플레이어 삭제 (RoomLeave 패킷 수신 시 삭제)
	void ClearOtherPlayers();						 // 모든 다른 플레이어 삭제 (게임 종료 시 삭제)
};