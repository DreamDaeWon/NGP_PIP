#include <windows.h> //--- 윈도우 헤더 파일
#include <tchar.h>
#include "MainGame.h"
#include "CameraManager.h"
#define WINSIZEX 800
#define WINSIZEY 600
HINSTANCE g_hInst;
HWND g_hWnd;
bool bShake{};
bool bZoom{};
bool bStop{};
bool bCombo{};
int iScore{};
LPCTSTR lpszClass = L"Window Class Name";
LPCTSTR lpszWindowName = L"Window Programming Lab";
CMainGame* MainGame;
LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpszCmdParam, int nCmdShow)
{
	
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc(2284);
	MSG Message;

	g_hInst = hInstance;

	WNDCLASSEX WndClass;
	WndClass.cbSize = sizeof(WndClass);
	WndClass.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS; // 더블클릭 추가
	WndClass.lpfnWndProc = (WNDPROC)WndProc;
	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hInstance = hInstance;
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	WndClass.lpszMenuName = NULL;
	WndClass.lpszClassName = lpszClass;
	WndClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	RegisterClassEx(&WndClass);

	RECT WinDow{ 0,0,WINSIZEX,WINSIZEY };
	AdjustWindowRect(&WinDow, WS_OVERLAPPEDWINDOW, FALSE);

	g_hWnd = CreateWindow(lpszClass, lpszWindowName, WS_OVERLAPPEDWINDOW,
		10, 10, WinDow.right - WinDow.left, WinDow.bottom - WinDow.top, NULL, (HMENU)NULL, hInstance, NULL);

	ShowWindow(g_hWnd, nCmdShow);


	UpdateWindow(g_hWnd);
	while (GetMessage(&Message, 0, 0, 0)) {
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}

	return Message.wParam;
}


enum TIME { TIME_PAINT, TIME_MOVE };

void CALLBACK TickTime(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime)
{
	
	if (!bStop)
	{
		MainGame->Update(0.01f);
		MainGame->LateUpdate(0.01f);
		InvalidateRect(hWnd, nullptr, false);
	}
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hDC, mDC{}, hBackGroundDC{}, hBackPlayerDC{};
	RECT rt{};
	TCHAR temp[] = TEXT("Hello world!");
	
	static int iShake{5};

	HBITMAP hBitmap{}, hBackGroundbitmap{} , hBackPlayerbitmap{};



	switch (iMessage) {

	case WM_CREATE:
	{
		MainGame = new CMainGame{};
		SetTimer(hWnd, TIME_PAINT, 1, (TIMERPROC)TickTime);

	}
		break;

	case WM_LBUTTONDOWN:

		break;


	case WM_LBUTTONUP:

		break;

	case WM_KEYDOWN:

		break;

		

	case WM_CHAR:
		if (wParam == 'o')
		{
			//bShake = !bShake;
			//bZoom = !bZoom;
			CCameraManager::GetInstance()->SetChangeLeftRight();
		}
		if (wParam == 'p')
		{
			//bStop = !bStop;
		}
		


		break;


	case WM_PAINT:
	{
		GetClientRect(hWnd, &rt);
		hDC = BeginPaint(hWnd, &ps);
		hBackGroundDC = CreateCompatibleDC(hDC);
		hBackGroundbitmap = CreateCompatibleBitmap(hDC, rt.right, rt.bottom);// 배경을 그릴 곳
		SelectObject(hBackGroundDC, hBackGroundbitmap);

		mDC = CreateCompatibleDC(hBackGroundDC);

		hBitmap = CreateCompatibleBitmap(hBackGroundDC, rt.right, rt.bottom);




		SelectObject(mDC, hBitmap); // mdc에 해주는 것 중요!
		// 테두리 지우는 코드 넣기
		HPEN hPen = (HPEN)CreatePen(PS_SOLID,2,RGB(255, 0, 255)), OldPen{};
		OldPen = (HPEN)SelectObject(mDC, hPen);

		HBRUSH hBrush = CreateSolidBrush(RGB(255,0,255)), OldBrush{};
		OldBrush = (HBRUSH)SelectObject(mDC, hBrush);

		Rectangle(mDC, 0, 0, rt.right, rt.bottom);

		SelectObject(mDC, OldBrush);
		SelectObject(mDC, OldPen);
		DeleteObject(hBrush);
		DeleteObject(hPen);
		// 여기서부터 그리기 하기!


		MainGame->Render(mDC);


		// 카메라
		CCameraManager::GetInstance()->SetBackDC(hBackGroundDC);
		CCameraManager::GetInstance()->Render(hDC, mDC);

		DeleteDC(mDC);
		DeleteDC(hBackGroundDC);
		DeleteObject(hBitmap);
		DeleteObject(hBackGroundbitmap);
		EndPaint(hWnd, &ps);
	}
		break;
	case WM_DESTROY:
	{
		delete MainGame;
		MainGame = nullptr;
	}
		PostQuitMessage(0);
		
		return 0;
	}

	return (DefWindowProc(hWnd, iMessage, wParam, lParam));
}