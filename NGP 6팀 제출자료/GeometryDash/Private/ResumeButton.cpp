#include "ResumeButton.h"
#include "CSoundManager.h"
#include "ObjManager.h"
#include "CollisionManager.h"
#include "KeyManager.h"
#include "StageManager.h"
#include "fstream"
#include "Tile.h"

CResumeButton::CResumeButton()
{
    Initailizer();
}

CResumeButton::~CResumeButton()
{
    Free();
}

void CResumeButton::Initailizer()
{
    LoadButtonSound();


    HBITMAP* InBitMap{};
    TCHAR Name[50] = {};

    wsprintf(Name, L"../resource/Button/PlayButton.bmp");
    //wsprintf(Name, L"../resource/Player/Gatto_Roll_%d.bmp",i);
    InBitMap = new HBITMAP;
    *InBitMap = (HBITMAP)LoadImage(g_hInst, Name, IMAGE_BITMAP, 128, 128, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
    m_vechBitMap.push_back(InBitMap);
    CSoundManager::GetInstance()->LoadSound("SelectSatage", "../sound/geometry-dash-level-selected.mp3");
}

int CResumeButton::Update(float fTime)
{

    return 0;
}

void CResumeButton::LateUpdate(float fTime)
{
    m_POSRECT.left = m_CenterPos.x - m_fRidius;
    m_POSRECT.right = m_CenterPos.x + m_fRidius;
    m_POSRECT.top = m_CenterPos.y - m_fRidius;
    m_POSRECT.bottom = m_CenterPos.y + m_fRidius;
    POINT Mouse{};
    GetCursorPos(&Mouse);
    ScreenToClient(g_hWnd, &Mouse);


    if (CCollisionManager::GetInstance()->CheckCircle_And_Point(this, Mouse) && CKeyManager::GetInstance()->KeyDown(VK_LBUTTON))
    {
        if (CStageManager::GetInstance()->GetNowSatge() == CStageManager::STAGE_TOOL)
        {
            // 이따 바꾸기
            SaveAllTile();
            CSoundManager::GetInstance()->SetAllvolume(0.f);

            CSoundManager::GetInstance()->PlayEffectSound("SelectSatage", CSoundManager::CHANNEL_OBJECT_SOUND1);
            CSoundManager::GetInstance()->SetVolume(CSoundManager::CHANNEL_OBJECT_SOUND1, 0.5f);
            Sleep(1300);
            CStageManager::GetInstance()->StageChanger(CStageManager::STAGE_TOOL_PLAY_STAGE);
        }
        else
        {
            CStageManager::GetInstance()->ResumeAll();
        }
    }
}

void CResumeButton::Render(HDC mDC)
{
    m_ButtonDc = CreateCompatibleDC(mDC);

    SelectObject(m_ButtonDc, *(m_vechBitMap[0]));

    TransparentBlt(mDC, m_CenterPos.x - m_fRidius,
        m_CenterPos.y - m_fRidius,
        m_fRidius * 2, m_fRidius * 2, m_ButtonDc, 0, 0, 128, 128, RGB(255, 0, 255));

    DeleteObject(m_ButtonDc);
}

void CResumeButton::Free()
{
    if (m_vechBitMap.size())
    {
        for (auto iter = m_vechBitMap.begin(); iter != m_vechBitMap.end(); ++iter)
        {
            DeleteObject(*(*iter));

            delete (*iter);
            (*iter) = nullptr;


        }
        m_vechBitMap.clear();
    }
}

void CResumeButton::LoadButtonSound()
{
    CSoundManager::GetInstance()->LoadSound("버튼", "../sound/geometry-dash-level-selected.mp3");
}

void CResumeButton::SaveAllTile()
{
    ofstream inStream("../SaveFile/EditSave.txt");
    if (inStream.fail())
    {
        inStream.close();
        return;
    }
    if (CObjManager::GetInstance()->GetAllVector()[CObjManager::OBJECT_TILE].size() == 0)
    {
        return;
    }

    string Instring{};
    for (auto iter = CObjManager::GetInstance()->GetAllVector()[CObjManager::OBJECT_TILE].begin();
        iter != CObjManager::GetInstance()->GetAllVector()[CObjManager::OBJECT_TILE].end();
        ++iter)
    {
        //stringstream iss(Instring);
        long INCenterPOSX = (*iter)->GetCenterPos().x;
        long INCenterPOSY = (*iter)->GetCenterPos().y;
        long INPOSRECTL = (*iter)->GetPosRect().left;
        long INPOSRECTT = (*iter)->GetPosRect().top;
        long INPOSRECTR = (*iter)->GetPosRect().right;
        long INPOSRECTB = (*iter)->GetPosRect().bottom;
        int INTILEType = dynamic_cast<CTile*>(*iter)->GetTileType();
        if (INTILEType >= CTile::TILE_POTAL_NOMAL_FRONT && INTILEType <= CTile::TILE_POTAL_LEFT_RIGHT_CHANGE_BACK)
        {
            if (INTILEType % 2 == 0) // back
            {
                INTILEType = (int)((INTILEType) / 2) * 2 - 1;
            }
            else // front
            {
                INTILEType = (int)((INTILEType) / 2) * 2 + 1;
            }
        }
        inStream << '\n' << INCenterPOSX << " " << INCenterPOSY << " " << INPOSRECTL << " " << INPOSRECTT << " " << INPOSRECTR << " "
            << INPOSRECTB << " " << INTILEType;
    }

    inStream.close();
}