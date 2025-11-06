#include "StageManager.h"
#include "StageOne.h"
#include "MainStage.h"
#include "MenuStage.h"
#include "ToolPlayStage.h"
#include "ToolStage.h"
#include "KeyManager.h"
#include "ObjManager.h"
#include "MainStage.h"
#include "MenuStage.h"
#include "CameraManager.h"
#include "StageTutorial.h"

CStageManager* CStageManager::m_pInstance = nullptr;

CStageManager::CStageManager()
{
    Initailizer();
}

CStageManager::~CStageManager()
{
    Free();
}

void CStageManager::Initailizer()
{
    m_NowStage = new CMainStage{};
    m_eNowStage = STAGE_MAIN;
}

int CStageManager::Update(float fTime)
{
    
    if (m_NowStage == nullptr)
    {
        return 0;
    }

    if (!m_bPlay)
    {
      
        return 0;
    }

    m_NowStage->Update(fTime);
    return 0;
}

void CStageManager::LateUpdate(float fTime)
{
    InKey();
    if (m_NowStage == nullptr)
    {
        return;
    }
    if (!m_bPlay)
    {

        ShowCursor(true);
    }

    m_NowStage->LateUpdate(fTime);
}

void CStageManager::Render(HDC mDC)
{
    if (m_NowStage == nullptr)
    {
        return;
    }
    if (!m_bPlay)
    {

        ShowCursor(true);
    }

    m_NowStage->Render(mDC);

}

void CStageManager::Free()
{
    if (m_NowStage != nullptr)
    {
        delete m_NowStage;
        m_NowStage = nullptr;
    }
}

void CStageManager::InKey()
{
    if (CKeyManager::GetInstance()->KeyUp(VK_ESCAPE) && !bEscMenu)
    {
        ShowCursor(true);
        CStageManager::GetInstance()->PauseAll();
        bEscMenu = true;
        switch (m_eNowStage)
        {
        case CStageManager::STAGE_ONE:
            dynamic_cast<CStageOne*>(m_NowStage)->CreatEscMenu();
            break;

        case CStageManager::STAGE_TUTORIAL:
            dynamic_cast<CStageTutorial*>(m_NowStage)->CreatEscMenu();
            break;

        case CStageManager::STAGE_TOOL:
            dynamic_cast<CToolStage*>(m_NowStage)->CreatEscMenu();

            break;
        case CStageManager::STAGE_TOOL_PLAY_STAGE:
            dynamic_cast<CToolPlayStage*>(m_NowStage)->CreatEscMenu();

            break;
        }
        CCameraManager::GetInstance()->ResetBackImageMove();
        return;
    }

    if (CKeyManager::GetInstance()->KeyUp(VK_ESCAPE) && bEscMenu)
    {
        ResumeAll();
        return;
    }
}

void CStageManager::StageChanger(STAGETYPE _Stage)
{
    CStageManager::GetInstance()->ResumeAll();
    switch (_Stage)
    {
    case CStageManager::STAGE_MAIN:
    {
        KillTimer(g_hWnd, 0);
        if (m_NowStage != nullptr)
        {
            delete m_NowStage;
            m_NowStage = nullptr;
        }
        SetTimer(g_hWnd, 0, 1, (TIMERPROC)TickTime);
        m_NowStage = new CMainStage{};
        m_eNowStage = STAGE_MAIN;
    }
    break;
    case CStageManager::STAGE_MENU:
    {
        KillTimer(g_hWnd, 0);
        if (m_NowStage != nullptr)
        {
            delete m_NowStage;
            m_NowStage = nullptr;
        }
        SetTimer(g_hWnd, 0, 1, (TIMERPROC)TickTime);
        m_NowStage = new CMenuStage{};
        m_eNowStage = STAGE_MENU;
    }
    break;
    case CStageManager::STAGE_TUTORIAL:
    {
        KillTimer(g_hWnd, 0);
        if (m_NowStage != nullptr)
        {
            delete m_NowStage;
            m_NowStage = nullptr;
        }
        SetTimer(g_hWnd, 0, 1, (TIMERPROC)TickTime);
        m_NowStage = new CStageTutorial{};
        m_eNowStage = STAGE_TUTORIAL;
    }
    break;
    case CStageManager::STAGE_ONE:
    {
        KillTimer(g_hWnd, 0);
        if (m_NowStage != nullptr)
        {
            delete m_NowStage;
            m_NowStage = nullptr;
        }
        SetTimer(g_hWnd, 0, 1, (TIMERPROC)TickTime);
        m_NowStage = new CStageOne{};
        m_eNowStage = STAGE_ONE;
    }
        break;
    case CStageManager::STAGE_TOOL:
    {
        KillTimer(g_hWnd, 0);
        if (m_NowStage != nullptr)
        {
            delete m_NowStage;
            m_NowStage = nullptr;
        }
        SetTimer(g_hWnd, 0, 1, (TIMERPROC)TickTime);

        m_NowStage = new CToolStage{};
        m_eNowStage = STAGE_TOOL;
    }
        break;
    case CStageManager::STAGE_TOOL_PLAY_STAGE:
    {
        KillTimer(g_hWnd, 0);
        if (m_NowStage != nullptr)
        {
            delete m_NowStage;
            m_NowStage = nullptr;
        }
        SetTimer(g_hWnd, 0, 1, (TIMERPROC)TickTime);

        m_NowStage = new CToolPlayStage{};
        m_eNowStage = STAGE_TOOL_PLAY_STAGE;
    }
        break;
    }
    CKeyManager::GetInstance()->ResetAllKey();
    CCameraManager::GetInstance()->ResetBackImageMove();
    CCameraManager::GetInstance()->SetNomalLeftRight();
}
