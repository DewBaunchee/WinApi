#include <windows.h>

#define TIMER_TICK (double) 35

struct Coordinates {
	double x, y;
};

struct MyRect {
	double x, y, width, height;
	Coordinates speed;

	HBITMAP hBmp;
	BITMAP bmp;
};

MyRect myRect, windowRect;
HDC hDC = 0;

COLORREF WHITE = RGB(255, 255, 255);

MyRect rectToMyRect(RECT rect) {
	MyRect myRect;
	myRect.x = (double)rect.left;
	myRect.y = (double)rect.top;
	myRect.width = (double)rect.right - rect.left;
	myRect.height = (double)rect.bottom - rect.top;
	return myRect;
}

double abs(double number) {
	return number > 0 ? number : -number;
}

void Move(Coordinates shift) {
	LONG windowBottom = windowRect.y + windowRect.height;
	LONG windowRight = windowRect.x + windowRect.width;

	double newX = myRect.x + shift.x;
	if (newX + myRect.width > windowRight) {
		newX -= 2 * (myRect.width - (windowRight - newX));
		myRect.speed.x *= -1;
	}
	if (newX < windowRect.x) {
		newX = abs(newX + windowRect.x);
		myRect.speed.x *= -1;
	}

	double newY = myRect.y + shift.y;
	if (newY + myRect.height > windowBottom) {
		newY -= 2 * (myRect.height - (windowBottom - newY));
		myRect.speed.y *= -1;
	}
	if (newY < windowRect.y) {
		newY = abs(newY + windowRect.y);
		myRect.speed.y *= -1;
	}

	myRect.x = newX;
	myRect.y = newY;
}

void ChangeSpeed(Coordinates delta) {
	myRect.speed.x += delta.x;
	myRect.speed.y += delta.y;
}

void LoadRect() {
	myRect.hBmp = (HBITMAP)LoadImage(NULL, L"img.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	GetObject(myRect.hBmp, sizeof(myRect.bmp), &myRect.bmp);

	myRect.width = myRect.bmp.bmWidth;
	myRect.height = myRect.bmp.bmHeight;
}

void DrawBitmap() {
	LoadRect();
	HDC hdcMem = CreateCompatibleDC(hDC);
	SelectObject(hdcMem, myRect.hBmp);

	HRGN rgn = CreateRoundRectRgn(myRect.x, myRect.y, myRect.x + myRect.width, myRect.y + myRect.height, 20, 20);
	SelectClipRgn(hDC, rgn);
	BitBlt(hDC, myRect.x, myRect.y, myRect.x + myRect.width, myRect.y + myRect.height, hdcMem, 0, 0, SRCCOPY);
}

void Clear() {
	HBRUSH whiteBrush = CreateSolidBrush(WHITE);
	SelectObject(hDC, whiteBrush);
	SelectObject(hDC, CreatePen(PS_SOLID, 0, WHITE));
	Rectangle(hDC, windowRect.x, windowRect.y, windowRect.x + windowRect.width, windowRect.y + windowRect.height);
}

void Redraw(HWND hWnd) {
	RECT window;
	GetClientRect(hWnd, &window);
	windowRect = rectToMyRect(window);

	Move({ myRect.speed.x * (TIMER_TICK / 1000), myRect.speed.y * (TIMER_TICK / 1000) });

	Clear();
	//SelectObject(hDC, CreateSolidBrush(RECT_COLOR));
	DrawBitmap();

	ValidateRect(hWnd, NULL);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	PAINTSTRUCT ps;

	switch (uMsg) {
	case WM_PAINT:
		hDC = BeginPaint(hWnd, &ps);
		LoadRect();
		SetTimer(hWnd, 0, TIMER_TICK, NULL);
	case WM_KEYDOWN:
		switch (wParam) {
		case VK_DOWN:
			ChangeSpeed({ 0, 10 });
			break;
		case VK_UP:
			ChangeSpeed({ 0, -10 });
			break;
		case VK_LEFT:
			ChangeSpeed({ -10, 0 });
			break;
		case VK_RIGHT:
			ChangeSpeed({ 10, 0 });
			break;
		}
		break;
	case WM_MOUSEWHEEL: {
		int fwKeys = GET_KEYSTATE_WPARAM(wParam);
		int delta = GET_WHEEL_DELTA_WPARAM(wParam);

		if (fwKeys == MK_SHIFT) {
			ChangeSpeed({ (double)delta, 0 });
		}
		else {
			ChangeSpeed({ 0, (double)delta });
		}
		break;
	}
	case WM_MOUSEHWHEEL: {
		int delta = GET_WHEEL_DELTA_WPARAM(wParam);

		ChangeSpeed({ (double)-delta, 0 });
		break;
	}
	case WM_TIMER: {
		Redraw(hWnd);
		break;
	}
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
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
	wc.lpszClassName = L"Rectangle";
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

	hMainWnd = CreateWindow(L"Rectangle",
		L"Moving bitmap",
		WS_OVERLAPPEDWINDOW | WS_VSCROLL,
		CW_USEDEFAULT, NULL, CW_USEDEFAULT, NULL,
		HWND(NULL), NULL, HINSTANCE(hInstance), NULL);

	if (!hMainWnd) {
		MessageBox(NULL, L"Cannot create a window", L"Error", MB_OK);
		return NULL;
	}

	ShowWindow(hMainWnd, nCmdShow);
	UpdateWindow(hMainWnd);

	MSG msg;
	while (GetMessage(&msg, NULL, NULL, NULL)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}
