#include "Handler.h"
#include "State.h"

#define state(hwnd) reinterpret_cast<State>(GetWindowLongPtr((hwnd), GWLP_USERDATA))
#define selectWindowState(hwnd) state(hwnd)->wndState
#define selectTable(hwnd) state(hwnd)->table
#define setShouldClear(hwnd) selectWindowState(hwnd).shouldClear = true;

#define TIMES_NEW_ROMAN L"Times New Roman"
#define Calibri L"Calibri"
#define ARIAL L"Arial"
#define SELECTED_FONT Calibri

#define ROWS 10
#define COLS 5
#define PADDING 5
#define SCROLL_SPEED 10

// ON CREATE
void FillTable(Table* table) {
	table->x = table->y = 0;
	table->rowCount = ROWS;
	table->colCount = COLS;
	table->padding = PADDING;

	table->cells = new String * [table->rowCount];
	for (int i = 0; i < table->rowCount; i++) {
		table->cells[i] = new String[table->colCount];
		for (int j = 0; j < table->colCount; j++) {
			table->cells[i][j].charSequence = L"QWERTY kl";
			table->cells[i][j].length = wcslen(table->cells[i][j].charSequence);
		}
	}
}

void OnCreate(WndMessage msg) {
	CREATESTRUCT* create = reinterpret_cast<CREATESTRUCT*>(msg.lParam);
	SetWindowLongPtr(msg.hwnd, GWLP_USERDATA, (LONG_PTR) reinterpret_cast<State>(create->lpCreateParams));
	FillTable(&selectTable(msg.hwnd));
}

// ON PAINT
void Clear(HWND hwnd) {
	RECT clientRect;
	GetClientRect(hwnd, &clientRect);
	InvalidateRect(hwnd, &clientRect, selectWindowState(hwnd).shouldClear);
	selectWindowState(hwnd).shouldClear = false;
}

void DrawTable(HDC hdc, WndState wndState, Table table) {
	int colWidth = wndState.width / table.colCount;

	SetTextColor(hdc, 0x00000000);
	SetBkMode(hdc, TRANSPARENT);

	HFONT font = CreateFont(30, 10, 0, 0, 
		FW_NORMAL, false, false, false, 
		ANSI_CHARSET, OUT_DEFAULT_PRECIS, 
		CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, 
		DEFAULT_PITCH, SELECTED_FONT);
	HGDIOBJ oldObj = SelectObject(hdc, font);

	int tableHeight = 0;
	int rowStartY = table.y;
	for (int i = 0; i < table.rowCount; i++) {

		int maxRegionHeight = 0;
		int colStartX = 0;

		for (int j = 0; j < table.colCount; j++) {
			RECT drawRegion;
			drawRegion.left = colStartX + table.padding;
			drawRegion.top = rowStartY + table.padding;
			drawRegion.right = colStartX + colWidth - table.padding;
			drawRegion.bottom = drawRegion.top + maxRegionHeight - table.padding;

			int newHeight = DrawText(hdc, const_cast<LPTSTR>(table.cells[i][j].charSequence), table.cells[i][j].length, &drawRegion, DT_EDITCONTROL | DT_WORDBREAK | DT_CALCRECT);
			if (maxRegionHeight < newHeight) {
				maxRegionHeight = newHeight;
				drawRegion.bottom = drawRegion.top + maxRegionHeight ;
			}

			DrawText(hdc, const_cast<LPTSTR>(table.cells[i][j].charSequence), table.cells[i][j].length, &drawRegion, DT_EDITCONTROL | DT_WORDBREAK);
			colStartX += colWidth;
		}

		rowStartY += maxRegionHeight + 2 * table.padding;
		tableHeight = rowStartY;
		MoveToEx(hdc, 0, rowStartY, NULL);
		LineTo(hdc, wndState.width, rowStartY);
	}

	int colStartX = colWidth;
	int colIndex = 1;

	while (colIndex++ < table.colCount) {
		MoveToEx(hdc, colStartX, table.y, NULL);
		LineTo(hdc, colStartX, tableHeight);
		colStartX += colWidth;
	}

	SelectObject(hdc, oldObj);
	DeleteObject(font);
}

void OnPaint(WndMessage msg) {
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(msg.hwnd, &ps);
	Clear(msg.hwnd);
	DrawTable(hdc, selectWindowState(msg.hwnd), selectTable(msg.hwnd));
	EndPaint(msg.hwnd, &ps);
}

// ON SIZE
void OnSize(WndMessage msg) {
	UINT width = LOWORD(msg.lParam);
	UINT height = HIWORD(msg.lParam);
	selectWindowState(msg.hwnd).width = width;
	selectWindowState(msg.hwnd).height = height;
}

// ON SCROLL
void OnVScroll(WndMessage msg) {
	selectTable(msg.hwnd).y = -HIWORD(msg.wParam) * SCROLL_SPEED;
	SetScrollPos(msg.hwnd, SB_VERT, HIWORD(msg.wParam), true);
	setShouldClear(msg.hwnd);
}

// ON DESTROY
void FreeState(State state) {
	Table table = state->table;
	for (int i = 0; i < table.rowCount; i++) {
		delete table.cells[i];
	}
	delete table.cells;

	delete state;
}

void OnDestroy(WndMessage msg) {
	FreeState(state(msg.hwnd));
	PostQuitMessage(0);
}
