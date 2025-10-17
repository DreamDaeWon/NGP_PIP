#pragma once
#include "Object.h"
#include "ObjManager.h"

class CStageManager : public CObject
{
public:
	CStageManager();
	~CStageManager();

public:
	// CObject을(를) 통해 상속됨
	void Initailizer() override;
	int Update(float fTime) override;
	void LateUpdate(float fTime) override;
	void Render(HDC mDC) override;
	void Free() override;

public:
	enum STAGETYPE {STAGE_MAIN,STAGE_MENU,STAGE_TUTORIAL,STAGE_ONE, STAGE_TOOL, STAGE_TOOL_PLAY_STAGE ,STAGE_END};
	void StageChanger(STAGETYPE _Stage);
	void PauseAll() { m_bPlay = false; }
	void ResumeAll() { m_bPlay = true; 
	CObjManager::GetInstance()->DeleteVector(CObjManager::OBECT_ESC_MENU);
	bEscMenu = false;
	}

	void SetEscMenu(bool _Menu)
	{
		bEscMenu = _Menu;
	}

	void InKey();

	int GetNowSatge(){ return m_eNowStage; }

public:
	static CStageManager* GetInstance()
	{
		if (m_pInstance == nullptr)
		{
			m_pInstance = new CStageManager{};
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


private:
	static CStageManager* m_pInstance;

private:
	class CStage* m_NowStage{};
	bool m_bPlay{true};
	bool bEscMenu{};

	STAGETYPE m_eNowStage{};
};

