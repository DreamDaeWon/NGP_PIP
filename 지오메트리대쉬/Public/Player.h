#pragma once
#include "Object.h"
#include "math.h"
#include "CSoundManager.h"

#define PI 3.141592
#define SpinSpeed 6

class CPlayer : public CObject
{
public:
    CPlayer();
    ~CPlayer() override;

    enum DIR { DIR_LEFT, DIR_RIGHT, DIR_UP, DIR_DOWN, DIR_SPACE, DIR_END };

    enum STATUS { STATUS_NOMAL, STATUS_AIRPLANE, STATUS_ZIGZAG, STATUS_FINISH, STATUS_DIE, STATUS_END };

    enum ZIGZIG_MODE { ZIGZAG_UP, ZIGZAG_DOWN };

public:

    void Initailizer() override;

    int Update(float fTime) override;

    void LateUpdate(float fTime) override;

    void Render(HDC mDC) override;

    void Free() override;

public:
    void KeyDown();

    void Collision();

    void MSpace(float fTime);

    float radian(float degrees);

    void LoadPlayerSound();

    //void SetStopSpin();

    void Set_Status(int _Status) { m_eStatus = (STATUS)_Status; }
    int Get_Status() { return  m_eStatus; }
    void SetAngle(float fDegrees) { angle = fDegrees; }

    void SetJump() {
        m_Jcnt = 0;
    }

    void SetJumpHeight() {
        m_JumpHeight = 25;
        Key[DIR_DOWN] = false;
        m_bMSpace = true;
    }



    //  void SetbMSpace(bool _Jump) { m_bMSpace = _Jump; }


    void Set_BeforeStatus(int _Status) {
        m_eBeforeStatus = (STATUS)_Status;
    }

private:
    vector<HBITMAP*> m_vechBitMap{};
    HDC m_PalyerDc{};

    STATUS m_eStatus{ STATUS_NOMAL }; // ���� �÷��̾� ����

    ZIGZIG_MODE m_eZigzag{ ZIGZAG_DOWN };

    STATUS m_eBeforeStatus{ STATUS_END }; // �� �÷��̾� ����

    float m_fTime{};

    bool Key[DIR_END]{};

    float m_fSpeed{ 600.f };

    float m_fJumpSpeed{ 800.f };

    DIR m_NowDir{ DIR_RIGHT };

    int m_Jcnt = 50; // ���� Ȯ�ο�

    int m_JumpHeight{ 15 };

    float m_Fcntx = 0; // �� x Ȯ�ο�
    float m_Fcnty = 0; // �� y Ȯ�ο�

    bool m_bMSpace = false; // ��� �߰� ���� Ȯ�ο�

    bool m_bFinish = false;

private:
    float angle{};

    float fspeed = 1.3f;

    POINT m_NowPoint[3]{ {(long)(0),(long)(0) },
       {(long)(m_fRidius * 2),(long)(m_fRidius * 2) },
       {(long)(0),(long)(m_fRidius * 2)} }; // ���� ���� �� ������ �� ���ʾƷ� �� ��ǥ

    HDC m_CompleteDc{};

};

