#ifndef STATE_H
#define STATE_H

typedef struct SString {
	const wchar_t* charSequence;
	int length;
} String;

typedef struct STable {
	int x;
	int y;

	int rowCount;
	int colCount;
	int padding;
	String** cells;
} Table;

typedef struct SWndState {
	int width;
	int height;

	bool shouldClear;
} WndState;

typedef struct SState {
	WndState wndState;
	Table table;
} * State;

#endif
