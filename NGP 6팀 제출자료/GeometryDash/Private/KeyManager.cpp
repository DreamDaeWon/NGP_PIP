#include "KeyManager.h"

CKeyManager* CKeyManager::m_pKeyManager = nullptr;

CKeyManager::CKeyManager()
{
}

CKeyManager::~CKeyManager()
{
}

void CKeyManager::Initailizer()
{
}

int CKeyManager::Update(float fTime)
{
    


    return 0;
}

void CKeyManager::LateUpdate(float fTime)
{
}

void CKeyManager::Render(HDC mDC)
{
    
}

void CKeyManager::Free()
{
}

bool CKeyManager::KeyDown(int _iKey)
{
    UpdatebKey(_iKey);
    // 이전에 눌림이 없음		&&	현재 눌렀음
    if (!m_bBeforeKey[_iKey]&& m_bKey[_iKey])
    {
      ///  memcpy(m_bBeforeKey, m_bKey, sizeof(m_bKey));
        return true;
    }
    else
    {
      //  memcpy(m_bBeforeKey, m_bKey, sizeof(m_bKey));
        return false;
    }
}

bool CKeyManager::KeyUp(int _iKey)
{

    UpdatebKey(_iKey);

    // 이전에 눌렀음		&&	현재 누르지 않음
    if (m_bBeforeKey[_iKey] && !m_bKey[_iKey])
    {
      //  memcpy(m_bBeforeKey, m_bKey, sizeof(m_bKey));
        return true;
    }


   // memcpy(m_bBeforeKey, m_bKey, sizeof(m_bKey));
    return false;
}

bool CKeyManager::KeyPressing(int _iKey)
{

    UpdatebKey(_iKey);
    if (GetAsyncKeyState(_iKey) & 0x8000)
    {
      //  memcpy(m_bBeforeKey, m_bKey, sizeof(m_bKey));
        return true;
    }
     // memcpy(m_bBeforeKey, m_bKey,sizeof(m_bKey));
    return false;
}

void CKeyManager::UpdatebKey(int _iKey)
{

   if ((GetAsyncKeyState(_iKey) & 0x8000))
   {
       m_bKey[_iKey] = true;
   }
   else if (!(GetAsyncKeyState(_iKey) & 0x8000))
   {
       m_bKey[_iKey] = false;
   }

}

void CKeyManager::UpdateBeforeKey()
{
    memcpy(m_bBeforeKey, m_bKey, sizeof(m_bKey));
}

void CKeyManager::ResetAllKey()
{
    memset(m_bKey,0, sizeof(m_bKey));
    memset(m_bBeforeKey,0, sizeof(m_bKey));
}

CKeyManager* CKeyManager::GetInstance()
{
    if (m_pKeyManager == nullptr)
    {
        m_pKeyManager = new CKeyManager{};
    }
    return m_pKeyManager;
}
