#ifndef HANDLER_H
#define HANDLER_H

#include <windows.h>

typedef struct SWndMessage {
	HWND hwnd;
	UINT uMsg;
	WPARAM wParam;
	LPARAM lParam;
} WndMessage;

void OnCreate(WndMessage msg);
void OnPaint(WndMessage msg);
void OnSize(WndMessage msg);
void OnVScroll(WndMessage msg);
void OnDestroy(WndMessage msg);

#endif