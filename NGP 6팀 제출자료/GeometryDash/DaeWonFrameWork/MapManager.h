#pragma once
#include "Singleton.h"

class MapManager : public Singleton<MapManager>
{
public:
	MapManager();
	~MapManager();

public:
	void GetInstance();
	void DestroyInstance();

	void MapShow();
	//void MapLoad();
	void MapStage();
};

