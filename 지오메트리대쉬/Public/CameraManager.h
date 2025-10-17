#pragma once
#include "Object.h"

class CCameraManager : public CObject
{
public:
	CCameraManager();
	virtual ~CCameraManager();


public:
	void Initailizer() override;
	int Update(float fTime) override;
	void LateUpdate(float fTime) override;
	void Render(HDC mDC) override;
	void Free() override;

public:
	void Render(HDC hDC, HDC mDC);

	void SetBackDC(HDC _BackDc);

	void SetBackBitMapImage(const TCHAR* _Name, int cx, int cy);


public:
	static CCameraManager* GetInstance()
	{
		if (m_pCameraManager == nullptr)
		{
			m_pCameraManager = new CCameraManager{};
		}

		return m_pCameraManager;
	}

	static void DestoryInstance()
	{
		delete m_pCameraManager;
		m_pCameraManager = nullptr;
	}


public:
	void SetPoint(POINT* _InPoint)
	{
		for (int i = 0; i < 3; ++i)
		{
			m_TargetPoint[i] = _InPoint[i];
		}
	}


public: // 바깥에 공개할 함수
	enum CAMERA_STATE {STATE_NONE, STATE_CHANGE_LEFTRIGHT, STATE_END};

	void SetChangeLeftRight(); // 좌우반전 하는 함수
	void SetNomalLeftRight(); // 좌우반전 되돌리는 함수

	void PlusMoveX(int _iSigh) // 배경을 오른쪽으로 돌리는 함수 인자값은 부호
	{
		m_BackImageMove.x += 1 * _iSigh; // 1 또는 2 또는 5 또는 10으로만 할것
		if (m_BackImageMove.x >= m_BackImageOriginalSize.x)
		{
			m_BackImageMove.x = 0;
		}
		if (m_BackImageMove.x <= 0)
		{
			m_BackImageMove.x = 0;
		}
	}

	void SetBackImageSize(int X, int Y) {
		m_BackImageSize.x = X; 
		m_BackImageSize.y = Y;
	}

	POINT GetBackImageSize()
	{
		return m_BackImageSize;
	}

	void ResetBackImageMove()
	{
		m_BackImageMove.x = 0;
		m_BackImageMove.y = 0;
	}


private: // 카메라 동작 내부 구현

	void ChangeLeftRight(float fTime); // 좌우 반전



private:
	static CCameraManager* m_pCameraManager;

	POINT InPointChangeLeftRight[3]{ {WINSIZEX,0 }, { 0,0 }, {WINSIZEX, WINSIZEY} }; // 좌우 반전

	POINT InPointOrigin[3]{ { 0,0 },{WINSIZEX,0 }, {0, WINSIZEY} }; // 기본 카메라

	POINT InPointTrun[3]{ { WINSIZEX / 2,0 },{WINSIZEX,WINSIZEY / 2 }, {0, WINSIZEY / 2} }; // 45도 회전 화면

	POINT InPointOriginZoom[3]{ { 0,0 },{WINSIZEX,0 }, {0, WINSIZEY} }; // 줌 화면


private: // 서서히 좌우 반전에 필요한 변수들


	bool m_bChangeLeftRight{}; // 좌우가 바뀌었는지?

	float m_fChangeLeftRightTime{2.f}; // 몇 초동안 바꿀건지?


private:
	CAMERA_STATE m_eNowState{ STATE_NONE }; // 현재 카메라 상태
	CAMERA_STATE m_eBeforeState{ STATE_NONE }; // 이전 카메라 상태

	HBITMAP m_hBitmapBackGround{}; // 배경 이미지 비트맵

	HDC m_hBackImageDC{}; // 배경 이미지를 선택할 DC

	POINT m_NowPoint[3]{ { 0,0 },{WINSIZEX,0 }, {0, WINSIZEY} }; // 현재 왼쪽 위 오른쪽 위 왼쪽아래 의 좌표

	POINT m_TargetPoint[3]{}; // 원하는 카메라 위 아래 좌표

	float m_fTime{}; // 현재 시간 값

	HDC m_BackDc{}; // 배경 DC

	POINT m_BackImageSize{}; // 배경 가로 세로 크기

	POINT m_BackImageMove{}; // 배경 이미지를 얼마나 이동시킬것인지?
	POINT m_BackImageOriginalSize{}; // 원본 배경 이미지 가로 세로 크기
	
};

