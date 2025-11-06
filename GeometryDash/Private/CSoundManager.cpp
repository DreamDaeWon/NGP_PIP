#include "CSoundManager.h"

CSoundManager* CSoundManager::m_pInstance = nullptr;

CSoundManager::CSoundManager()
{
	Initailizer();
}

CSoundManager::~CSoundManager()
{
	Free();
}

void CSoundManager::Initailizer()
{
	m_Result = FMOD::System_Create(&m_ssystem); //--- 사운드 시스템 생성
	if (m_Result != FMOD_OK)
		exit(0);

	// 초기화
	m_ssystem->init(32, FMOD_INIT_NORMAL, m_extradriverdata);
}

int CSoundManager::Update(float fTime)
{
	return 0;
}

void CSoundManager::LateUpdate(float fTime)
{
}

void CSoundManager::Render(HDC mDC)
{
}

void CSoundManager::Free()
{

	for(int i = 0; i< CHANNEL_END; ++i)
	{ 
	
		m_Channel[i]->stop();


	}

	for (auto iter = m_mapSound.begin(); iter != m_mapSound.end(); ++iter)
	{
		(*iter).second->release();
		//delete ((*iter).second);
		(*iter).second = nullptr;
	}

	m_mapSound.erase(m_mapSound.begin(), m_mapSound.end());

	m_ssystem->close();

	m_ssystem->release();
}

void CSoundManager::DeletaAllSound()
{
	for (int i = 0; i < CHANNEL_END; ++i)
	{

		m_Channel[i]->stop();


	}

	for (auto iter = m_mapSound.begin(); iter != m_mapSound.end(); ++iter)
	{
		//delete (*iter).second;
		//(*iter).second = nullptr;
	}

	m_mapSound.erase(m_mapSound.begin(), m_mapSound.end());
}

void CSoundManager::LoadSound(const string _Name, const string _path)
{
	if (m_mapSound.end() != m_mapSound.find(_Name))
	{
		return;
	}

	FMOD::Sound* ChangeSound{};
	m_ssystem->createSound(_path.c_str(), FMOD_LOOP_NORMAL, 0, &ChangeSound);
	FMOD::Sound* INSound = ChangeSound;
	//_path.assign
	m_mapSound.insert(make_pair(_Name, INSound));

	

	

}

void CSoundManager::PlayBGM(const string _Name)
{
	auto iter = m_mapSound.find(_Name);
	if (iter == m_mapSound.end())
	{
		return;
	}



	m_Channel[CHANNEL_BGM]->stop();
	m_ssystem->playSound((*iter).second,0,false, &m_Channel[CHANNEL_BGM]);
	m_Channel[CHANNEL_BGM]->setMode(FMOD_LOOP_NORMAL);

}

void CSoundManager::PlayEffectSound(const string _Name, CHANNEL _CHANNEL)
{
	auto iter = m_mapSound.find(_Name);
	if (iter == m_mapSound.end())
	{
		return;
	}

	m_Channel[_CHANNEL]->stop();
	m_ssystem->playSound((*iter).second, 0, false, &m_Channel[_CHANNEL]);
	m_Channel[_CHANNEL]->setMode(FMOD_LOOP_OFF);
}

void CSoundManager::SetVolume(int _Channel, float _Volume)
{
	m_Channel[_Channel]->setVolume(_Volume);
}

void CSoundManager::SetAllvolume(float _Volume)
{
	for (int i = CHANNEL_BGM; i < CHANNEL_END; ++i)
	{
		m_Channel[i]->setVolume(_Volume);
	}
}

