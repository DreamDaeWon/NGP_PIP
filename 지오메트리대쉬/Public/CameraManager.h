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


public: // �ٱ��� ������ �Լ�
	enum CAMERA_STATE {STATE_NONE, STATE_CHANGE_LEFTRIGHT, STATE_END};

	void SetChangeLeftRight(); // �¿���� �ϴ� �Լ�
	void SetNomalLeftRight(); // �¿���� �ǵ����� �Լ�

	void PlusMoveX(int _iSigh) // ����� ���������� ������ �Լ� ���ڰ��� ��ȣ
	{
		m_BackImageMove.x += 1 * _iSigh; // 1 �Ǵ� 2 �Ǵ� 5 �Ǵ� 10���θ� �Ұ�
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


private: // ī�޶� ���� ���� ����

	void ChangeLeftRight(float fTime); // �¿� ����



private:
	static CCameraManager* m_pCameraManager;

	POINT InPointChangeLeftRight[3]{ {WINSIZEX,0 }, { 0,0 }, {WINSIZEX, WINSIZEY} }; // �¿� ����

	POINT InPointOrigin[3]{ { 0,0 },{WINSIZEX,0 }, {0, WINSIZEY} }; // �⺻ ī�޶�

	POINT InPointTrun[3]{ { WINSIZEX / 2,0 },{WINSIZEX,WINSIZEY / 2 }, {0, WINSIZEY / 2} }; // 45�� ȸ�� ȭ��

	POINT InPointOriginZoom[3]{ { 0,0 },{WINSIZEX,0 }, {0, WINSIZEY} }; // �� ȭ��


private: // ������ �¿� ������ �ʿ��� ������


	bool m_bChangeLeftRight{}; // �¿찡 �ٲ������?

	float m_fChangeLeftRightTime{2.f}; // �� �ʵ��� �ٲܰ���?


private:
	CAMERA_STATE m_eNowState{ STATE_NONE }; // ���� ī�޶� ����
	CAMERA_STATE m_eBeforeState{ STATE_NONE }; // ���� ī�޶� ����

	HBITMAP m_hBitmapBackGround{}; // ��� �̹��� ��Ʈ��

	HDC m_hBackImageDC{}; // ��� �̹����� ������ DC

	POINT m_NowPoint[3]{ { 0,0 },{WINSIZEX,0 }, {0, WINSIZEY} }; // ���� ���� �� ������ �� ���ʾƷ� �� ��ǥ

	POINT m_TargetPoint[3]{}; // ���ϴ� ī�޶� �� �Ʒ� ��ǥ

	float m_fTime{}; // ���� �ð� ��

	HDC m_BackDc{}; // ��� DC

	POINT m_BackImageSize{}; // ��� ���� ���� ũ��

	POINT m_BackImageMove{}; // ��� �̹����� �󸶳� �̵���ų������?
	POINT m_BackImageOriginalSize{}; // ���� ��� �̹��� ���� ���� ũ��
	
};

