#include "CollisionManager.h"

CCollisionManager* CCollisionManager::m_pInstance = nullptr;

CCollisionManager::CCollisionManager()
{
    Initailizer();
}

CCollisionManager::~CCollisionManager()
{
    Free();
}

void CCollisionManager::Initailizer()
{
}

int CCollisionManager::Update(float fTime)
{
    return 0;
}

void CCollisionManager::LateUpdate(float fTime)
{
}

void CCollisionManager::Render(HDC mDC)
{
}

void CCollisionManager::Free()
{
}

bool CCollisionManager::CheckCircle(CObject* _Dst, CObject* _Src)
{
    // ���� �� ��ü ������ �Ÿ��� ���Ѵ�.
    float fDistance = sqrtf((_Dst->GetCenterPos().x - _Src->GetCenterPos().x) * (_Dst->GetCenterPos().x - _Src->GetCenterPos().x)
        + (_Dst->GetCenterPos().y - _Src->GetCenterPos().y) * (_Dst->GetCenterPos().y - _Src->GetCenterPos().y));

    // ���� �Ÿ��� ���Ѵ�.
    float fStandardDistance = _Dst->GetRidius() + _Src->GetRidius();


    if (fStandardDistance >= fDistance) // �浹
    {
        return true;
    }
    else // �浹 ����
    {
        return false;
    }
}

bool CCollisionManager::CheckCircle_And_Point(CObject* _Dst, POINT _Src)
{
    // ���� �� ��ü ������ �Ÿ��� ���Ѵ�.
    float fDistance = sqrtf((_Dst->GetCenterPos().x - _Src.x) * (_Dst->GetCenterPos().x - _Src.x)
        + (_Dst->GetCenterPos().y - _Src.y) * (_Dst->GetCenterPos().y - _Src.y));

    // ���� �Ÿ��� ���Ѵ�.
    float fStandardDistance = _Dst->GetRidius();


    if (fStandardDistance >= fDistance) // �浹
    {
        return true;
    }
    else // �浹 ����
    {
        return false;
    }
}

bool CCollisionManager::CheckRect(CObject* _Dst, CObject* _Src)
{
    RECT NullRect{};
    RECT Dst = _Dst->GetPosRect();
    RECT Src = _Src->GetPosRect();


    if (IntersectRect(&NullRect, &Dst, &Src))
    {
        return true;
    }
    else
    {
        return false;
    }
}
