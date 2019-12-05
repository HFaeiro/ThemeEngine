#pragma once

#include "ThemeEngine.h"

#define IDB_REFRESH (WM_USER + 1343)
#define IDB_REMOVE	(WM_USER + 1344)
#define IDB_PIN		(WM_USER + 1345)
#define IDC_CLEAR	(WM_USER + 1346)
#define IDR_GETLIST	(WM_USER + 1347)
#define IDB_ADD		(WM_USER + 1348)
#define IDB_SORTACE	(WM_USER + 1349)
#define IDB_SORTDEC	(WM_USER + 1350)

LRESULT __stdcall ListViewButtonProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT ListViewProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void InitializeListView(HINSTANCE hInstance, HBRUSH backgroundcolor);