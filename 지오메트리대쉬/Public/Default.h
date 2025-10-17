#pragma once

#include <iostream>
#include<Windows.h>
#include "tchar.h"
#include <vector>
#include <list>
#include <map>


#define WINSIZEX 800
#define WINSIZEY 600


#define TileSizeX 50 // 타일의 가로 길이
#define TileSizeY 50 // 타일의 세로 길이

#define VK_MAX 0xff

#define AllTileNum 17 // 모든 사용 타일 종류의 수


using namespace std;

extern HWND g_hWnd;

extern HINSTANCE g_hInst;

extern bool bShake;

extern bool bCombo;

extern int iScore;

extern void  CALLBACK TickTime(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime);