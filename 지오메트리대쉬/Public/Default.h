#pragma once

#include <iostream>
#include<Windows.h>
#include "tchar.h"
#include <vector>
#include <list>
#include <map>


#define WINSIZEX 800
#define WINSIZEY 600


#define TileSizeX 50 // Ÿ���� ���� ����
#define TileSizeY 50 // Ÿ���� ���� ����

#define VK_MAX 0xff

#define AllTileNum 17 // ��� ��� Ÿ�� ������ ��


using namespace std;

extern HWND g_hWnd;

extern HINSTANCE g_hInst;

extern bool bShake;

extern bool bCombo;

extern int iScore;

extern void  CALLBACK TickTime(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime);