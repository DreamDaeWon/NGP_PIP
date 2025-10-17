#pragma once
//#pragma comment (lib, "fmod_vc.lib")
#include "fmod.hpp"
#include "fmod_errors.h"
#include "Object.h"


class CSoundManager : public CObject
{
public:
	CSoundManager();
	~CSoundManager();

public:

	// CObject을(를) 통해 상속됨
	void Initailizer() override;

	int Update(float fTime) override;

	void LateUpdate(float fTime) override;

	void Render(HDC mDC) override;

	void Free() override;

public:
	static CSoundManager* GetInstance()
	{
		if (m_pInstance == nullptr)
		{
			m_pInstance = new CSoundManager{};
		}
		return m_pInstance;
	}

	static void DestoryInstance()
	{
		delete m_pInstance;
		m_pInstance = nullptr;
	}
	
public:
	enum CHANNEL {CHANNEL_BGM, CHANNEL_NOMAL_SOUND, CHANNEL_PLAYER_SOUND,
		CHANNEL_OBJECT_SOUND1, CHANNEL_OBJECT_SOUND2, CHANNEL_OBJECT_SOUND3,
		CHANNEL_END
	};

public:
	void DeletaAllSound();

public:
	void LoadSound(const string _Name, const string _path);

	void PlayBGM(const string _Name);

	void PlayEffectSound(const string _Name, CHANNEL _CHANNEL);

	void SetVolume(int _Channel,float _Volume);

	void SetAllvolume(float _Volume);

private:
	static CSoundManager* m_pInstance;

	FMOD::Channel* m_Channel[CHANNEL_END]{};

	FMOD::System* m_ssystem;
	map<const string, FMOD::Sound*> m_mapSound;
	//vector<FMOD::Sound*> m_vecSound[CHANNEL_END];
	FMOD_RESULT m_Result{};
	void* m_extradriverdata = NULL;
};

