#pragma once
class UIManager
{
public:
	UIManager();
	~UIManager();

public:
	void GetInstance();
	void DestroyInstance();

	void MiniMapUpdate();
	void TimerUpdate();

	void MiniMapShow();
	//void ScoreBoardShow(); <- CJ 질문 : score board 없는거지???
	void TimerShow();
	void UserNumShow();

	
};

