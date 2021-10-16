#include <windows.h>
#include "State.h"
#include "Handler.h"

#define CLASS_NAME L"Table"
#define WINDOW_TITLE L"Resizable table"

LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	WndMessage msg;
	msg.hwnd = hwnd;
	msg.uMsg = uMsg;
	msg.wParam = wParam;
	msg.lParam = lParam;

	switch (uMsg) {
	case WM_CREATE:
		OnCreate(msg);
		break;
	case WM_PAINT:
		OnPaint(msg);
		break;
	case WM_SIZE:
		OnSize(msg);
		break;
	case WM_VSCROLL:
		OnVScroll(msg);
		break;
	case WM_DESTROY:
		OnDestroy(msg);
		break;
	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR plCmdLine,
	int nCmdShow) {

	HWND hMainWnd;
	WNDCLASSEX wc;
	wc.cbSize = sizeof(wc);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = CLASS_NAME;
	wc.cbWndExtra = NULL;
	wc.cbClsExtra = NULL;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = LoadIcon(NULL, IDI_WINLOGO);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.hInstance = hInstance;

	if (!RegisterClassEx(&wc)) {
		MessageBox(NULL, L"Window not registered", L"Error", MB_OK);
		return NULL;
	}

	State state = new SState();

	hMainWnd = CreateWindowEx(
		0,
		CLASS_NAME,
		WINDOW_TITLE,
		WS_OVERLAPPEDWINDOW | WS_VSCROLL,
		CW_USEDEFAULT, NULL, CW_USEDEFAULT, NULL,
		HWND(NULL),
		NULL,
		HINSTANCE(hInstance),
		state
	);

	if (!hMainWnd) {
		MessageBox(NULL, L"Cannot create a window", L"Error", MB_OK);
		return NULL;
	}

	ShowWindow(hMainWnd, nCmdShow);

	MSG msg;
	BOOL bRet = 0;
	while (bRet = GetMessage(&msg, NULL, NULL, NULL))
	{
		if (-1 == bRet) break;
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}
