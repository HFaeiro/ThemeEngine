#pragma once
//#include "framework.h"
#include <windows.h>
#include <vector>
#include <string>
#include <algorithm>




namespace Theme {
#define CS_EXIT		1 << 0
#define CS_MINI		1 << 1
#define CS_MAXI		1 << 2
#define CS_TAB		1 << 3
#define CS_RECORD	1 << 4
#define TAB			(WM_USER + 1337)
#define GETFOCUS	(WM_USER + 1338)
#define LV_REFRESH	(WM_USER + 1339)
#define LV_REMOVE	(WM_USER + 1340)
#define LV_PIN		(WM_USER + 1341)

	extern int SCREEN_WIDTH;
	extern int SCREEN_HEIGHT;

	RECT DrawTextC(RECT, HDC, LPCWSTR, int, BOOL);
	RECT GetRect(HWND hWnd);



	HWND CWindow(RECT, LPCWSTR, LPCTSTR lpClassName, LPCTSTR lpWindowName, DWORD dwStyle, int x, int y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu);

	void Initialize(HINSTANCE hInstance, HBRUSH backgroundcolor);

	HANDLE CreateCFont(RECT, LPCWSTR);

	ATOM RClass(UINT style, WNDPROC wP, HINSTANCE hInstance, HCURSOR hCur, HICON hIcon, HBRUSH hbBG, LPCWSTR menuName, LPCWSTR ClassName, int size);
	LRESULT CALLBACK buttonProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK editProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	

	void DrawEdge(HDC, RECT&);


	struct ControlData {
		DWORD dwStyle;
		HWND hWnd, hFocus, parent;
		RECT csize, TxtDem;
		WCHAR OGT[99], Text[99];
		int len, hMenu;
		BOOL FOCUS, BMT, ShiftState, HighLight;
		HANDLE font;
	};


	struct ListViewButton {
		BOOL Pinned = FALSE;
		int ButtonNumber;
		ControlData* pData;
		std::vector<std::wstring> cData;
		~ListViewButton() { DestroyWindow(pData->hWnd); }
	};

	struct ListView {

		int counter, ButtonHeight, ButtonWidth, ControlHeight, Style, max, numOfPinned;
		BOOL VSCROLL;
		std::vector<ListViewButton*> vLVButton;
		std::vector<std::vector<std::wstring>> vListData;
	};
	typedef struct ControlData CData;

	
};

