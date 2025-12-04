#include "Default.h"
#include "MainGame.h"
#include "Packet.h"
#include "CameraManager.h"
#include "../DaeWonFrameWork/TimerManager.h"
#include "../DaeWonFrameWork/NetworkManager.h"
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
std::wstring SERVER_IP = L"127.0.0.1";
CMainGame* MainGame;
LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK IPWndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);

void DoIPInput(HINSTANCE hInstance) {
	WNDCLASSEX wc = { sizeof(WNDCLASSEX),
		CS_HREDRAW | CS_VREDRAW, 
		IPWndProc,0, 0,
		hInstance, NULL,
		LoadCursor(NULL,IDC_ARROW),
		(HBRUSH)(COLOR_WINDOW + 1),
		NULL,
		L"IPInputClass", 
		NULL };
	RegisterClassEx(&wc);

	int scrWidth = GetSystemMetrics(SM_CXSCREEN);
	int scrHeight = GetSystemMetrics(SM_CYSCREEN);
	int w = 300, h = 150;
	int x = (scrWidth - w) / 2;
	int y = (scrHeight - h) / 2;

	HWND hWnd = CreateWindow(L"IPInputClass", L"Enter Server IP", 
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_VISIBLE,
		x, y, w, h, NULL, NULL, hInstance, NULL);

	// 이 부분에서 InputBox가 모달(modal)로 동작하도록 메시지 루프를 실행합니다.
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		if (msg.message == WM_QUIT) break;
	}
	UnregisterClass(L"IPInputClass", hInstance);
}

LRESULT CALLBACK IPWndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam) {
	static HWND hEdit; // EDIT 컨트롤 핸들
	switch (iMessage) {
	case WM_CREATE:
		// IP 주소 텍스트 라벨 생성
		CreateWindow(L"STATIC", L"IP Address:", WS_CHILD | WS_VISIBLE, 20, 25, 80, 20, hWnd, NULL, NULL, NULL);
		// IP 주소 입력 에디트 컨트롤 생성
		hEdit = CreateWindowW(L"EDIT", SERVER_IP.c_str(), WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL, 100, 25, 160,
			20, hWnd, (HMENU)101, NULL, NULL);
		// 확인 버튼 생성
		CreateWindow(L"BUTTON", L"Connect", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON ,
			10, 60, 100, 30, hWnd, (HMENU)IDOK,
			NULL, NULL);
		return 0;
	case WM_COMMAND:
		// 버튼 클릭 처리
		if (LOWORD(wParam) == IDOK) { // Connect 버튼이 눌렸을 때
			int len = GetWindowTextLength(hEdit);
			if (len > 0) {
				std::vector<wchar_t> buf(len + 1); // 입력된 텍스트를 저장할 버퍼
				GetWindowText(hEdit, buf.data(), len + 1);
				SERVER_IP = buf.data(); // 전역 변수 SERVER_IP에 저장
			}
			DestroyWindow(hWnd); // [수정됨] 창을 파괴합니다
		}
		return 0;
	case WM_CLOSE:
		DestroyWindow(hWnd); // [수정됨] 닫기 버튼(X)을 눌러도 창을 파괴합니다.
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hWnd, iMessage, wParam, lParam);
}
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpszCmdParam, int nCmdShow)
{
	int argc = 0;

	/*LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);

	if (argv == NULL)
	{
		for (int i = 0; i < argc; i++)
		{
			MessageBox(NULL, L"로컬 호스트로 connect 시도", L"Argv Check", MB_OK);
		}
		SERVER_IP = L"127.0.0.1";
	}
	else
	{
		SERVER_IP = argv[1];
	}*/
	
	DoIPInput(hInstance);
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	MSG Message;

	g_hInst = hInstance;

	WNDCLASSEX WndClass;
	WndClass.cbSize = sizeof(WndClass);
	WndClass.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS; 
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
	
	// --- 게임 루프와 고정밀 타이머 설정 ---
	LARGE_INTEGER liFrequency{};
	LARGE_INTEGER liPrevCount{};
	LARGE_INTEGER liCurCount{};

	// 초당 클럭 수
	QueryPerformanceFrequency(&liFrequency);
	// 이전 클럭 수
	QueryPerformanceCounter(&liPrevCount);

	while (true)
	{
		TimerManager::Instance()->Tick();
		// 메시지 처리
		if (PeekMessage(&Message, NULL, 0, 0, PM_REMOVE))
		{
			if (Message.message == WM_QUIT)
				break;

			TranslateMessage(&Message);
			DispatchMessage(&Message);
		}
		// 메시지가 없을 때 게임 로직 및 렌더링 실행
		else
		{

			if (!bStop)
			{
				NetworkManager::Instance()->updatePacket();
				MainGame->Update(TimerManager::Instance()->GetDeltaTime()); // 직접 계산한 fTime 전달
				MainGame->LateUpdate(TimerManager::Instance()->GetDeltaTime()); // 직접 계산한 fTime 전달
				InvalidateRect(g_hWnd, nullptr, false); // 화면 갱신 요청 -> WM_PAINT 호출
			}
		}
	}

	return Message.wParam;
}


enum TIME { TIME_PAINT, TIME_MOVE };

// TickTime 콜백 함수는 더 이상 사용되지 않으므로 주석 처리합니다.

void CALLBACK TickTime(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime)
{
	
	if (!bStop)
	{
		TimerManager::Instance()->Tick();
		NetworkManager::Instance()->updatePacket();
		MainGame->Update(TimerManager::Instance()->GetDeltaTime());
		MainGame->LateUpdate(TimerManager::Instance()->GetDeltaTime());
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
		// -> 새로운 게임 루프에서 델타타임을 직접 계산하므로 더 이상 필요 없습니다.
		// TimerManager::Instance()->init();
		// SetTimer는 더 이상 사용하지 않으므로 주석 처리합니다.
		// SetTimer(hWnd, TIME_PAINT, 1, (TIMERPROC)TickTime);
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
		hBackGroundbitmap = CreateCompatibleBitmap(hDC, rt.right, rt.bottom);//  ׸ 
		SelectObject(hBackGroundDC, hBackGroundbitmap);

		mDC = CreateCompatibleDC(hBackGroundDC);

		hBitmap = CreateCompatibleBitmap(hBackGroundDC, rt.right, rt.bottom);




		SelectObject(mDC, hBitmap); // mdc ִ  ߿!
		// ׵θ  ڵ ֱ
		HPEN hPen = (HPEN)CreatePen(PS_SOLID,2,RGB(255, 0, 255)), OldPen{};
		OldPen = (HPEN)SelectObject(mDC, hPen);

		HBRUSH hBrush = CreateSolidBrush(RGB(255,0,255)), OldBrush{};
		OldBrush = (HBRUSH)SelectObject(mDC, hBrush);

		Rectangle(mDC, 0, 0, rt.right, rt.bottom);

		SelectObject(mDC, OldBrush);
		SelectObject(mDC, OldPen);
		DeleteObject(hBrush);
		DeleteObject(hPen);
		// ⼭ ׸ ϱ!


		MainGame->Render(mDC);


		// ī޶
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
