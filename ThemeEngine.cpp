// ThemeEngine.cpp : Defines the functions for the static library.
//

#include "framework.h"
#include "ThemeEngine.h"


namespace Theme
{

	int SCREEN_WIDTH;
	int SCREEN_HEIGHT;
};

void Theme::Initialize(HINSTANCE hInstance, HBRUSH backgroundcolor)
{
	
	RClass(CS_HREDRAW | CS_VREDRAW, Theme::buttonProc, hInstance,
		LoadCursor(nullptr, IDC_ARROW), NULL,
		backgroundcolor, NULL, L"CBUTTON", sizeof(Theme::CData));

	RClass(CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS, Theme::editProc, hInstance, LoadCursor(nullptr, IDC_IBEAM), NULL,
		backgroundcolor, NULL, L"CEDIT", sizeof(Theme::CData));
}



HANDLE Theme::CreateCFont(RECT r, LPCWSTR wstr) //easy createfont function.
{
	return CreateFont(r.left, r.right, r.top, r.bottom, FW_EXTRABOLD, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		PROOF_QUALITY, DEFAULT_PITCH | FF_DONTCARE, wstr);
}

RECT Theme::DrawTextC(RECT r, HDC hdc, LPCWSTR staticText, int len, BOOL Center) //draw text in window. 
{

	UINT format = DT_CENTER;
	RECT rt = r;
	/*int height = */DrawText(hdc, staticText, len, &rt, format | DT_CALCRECT);
	//rt.right = r.right;
	if (rt.bottom < r.bottom)
		OffsetRect(&rt, 0, (r.bottom - rt.bottom) / 2);
	if (Center)
	{
		rt.left -= (rt.right - rt.left);
	}
	DrawText(hdc, staticText, len, &rt, format);
	return rt;
}



RECT Theme::GetRect(HWND hWnd) //get exact dementions of certain window. 
{
	RECT r = { 0 };
	GetWindowRect(hWnd, &r);
	r.right = r.right - r.left;
	r.left = 0;
	r.bottom = r.bottom - r.top;
	r.top = 0;
	return r;
}

LRESULT CALLBACK Theme::buttonProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) //button procedure.
{
	static BOOL LBD = FALSE;

	CData* pData = (CData*)GetWindowLongPtr(hWnd, GWLP_USERDATA); //get button data pointer.
	switch (message)
	{

	case WM_RBUTTONDOWN:
	{
		if (pData->dwStyle & CS_TAB) {
			POINT cursor = { 0 };
			GetCursorPos(&cursor);
			HMENU popupMenu = CreatePopupMenu();
			int screens = SendMessage(GetParent(hWnd), TAB, NULL, (LPARAM)hWnd);
			

		}
		break;
	}
	case WM_ERASEBKGND: //on erase background. 
	{
		if (pData->BMT && ((pData->dwStyle & CS_EXIT) || (pData->dwStyle & CS_MINI) || (pData->dwStyle & CS_MAXI))) //currently only handling Exit, Minimize, and maximize buttons.
		{
			RECT r = GetRect(hWnd);
			HDC hdc = (HDC)wParam;


			FillRect(hdc, &r, (pData->dwStyle & CS_EXIT) ? CreateSolidBrush(RGB(0x9C, 0x1C, 0x1C)) : CreateSolidBrush(RGB(0x4C, 0x4C, 0x4C))); //fill the button background with corresponding color.

			break;
		}
		else return DefWindowProc(hWnd, message, wParam, lParam);
	}
	case GETFOCUS: //returns whether or not button has focus.
	{
		BOOL IB = (!pData->FOCUS ? 0 : 1);
		return IB;
	}
	case WM_MOUSEACTIVATE: //if button was clicked.
	{

		if (!(pData->dwStyle & CS_EXIT) && !(pData->dwStyle & CS_MINI) && !(pData->dwStyle & CS_MAXI) && (!(pData->dwStyle & CS_TAB) || pData->dwStyle & CS_TAB && lParam)) { //if button isn't exit or minimize set focus on or off.
			if (pData->FOCUS) {
				pData->FOCUS = FALSE;
				pData->BMT = FALSE;

				InvalidateRect(hWnd, 0, TRUE);
			}

			else
			{
				pData->FOCUS = TRUE;
				pData->BMT = FALSE;
				InvalidateRect(hWnd, 0, TRUE);
			}

		}
		if (lParam != 1 && (!(pData->dwStyle & CS_TAB) /*|| pData->dwStyle & CS_TAB && lParam*/)) //tell the main window the button was clicked. 
			SendMessage(GetParent(hWnd), WM_COMMAND, LOWORD(pData->hMenu), 0);
		break;
	}
	case WM_LBUTTONDOWN:
	{
		if (pData->dwStyle & CS_TAB)
		{

			SendMessage(GetParent(hWnd), WM_COMMAND, LOWORD(pData->hMenu), (LPARAM)hWnd);
			pData->FOCUS = !pData->FOCUS;
			InvalidateRect(hWnd, 0, TRUE);
			break;
		}
		else return DefWindowProc(hWnd, message, wParam, lParam);

	}
	case WM_MOUSEMOVE: //if mouse moved over button set track event to true.
	{
		if (!pData->BMT)
		{
			TRACKMOUSEEVENT TME = { NULL };
			TME.cbSize = sizeof(TRACKMOUSEEVENT);
			TME.dwFlags = TME_LEAVE;
			TME.hwndTrack = hWnd;
			TME.dwHoverTime = NULL;
			pData->BMT = TrackMouseEvent(&TME);
			InvalidateRect(hWnd, 0, TRUE);
		}

		break;
	}
	case WM_MOUSELEAVE: //if mouse left button area set track to false.
	{
		pData->BMT = FALSE; //hover == false
		InvalidateRect(hWnd, 0, TRUE);

	}
	break;

	case WM_PAINT: //on draw button
	{
		if (pData == NULL)
			return FALSE;
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);

		RECT r = GetRect(hWnd);
		HPEN hp;
		int i = SaveDC(hdc);

		if (!(pData->dwStyle & CS_EXIT) && !(pData->dwStyle & CS_MINI) && !(pData->dwStyle & CS_MAXI) && !(pData->dwStyle & CS_RECORD))
		{

			SetTextColor(hdc, pData->BMT || pData->FOCUS ? RGB(2, 244, 235) : RGB(200, 200, 200));
			WCHAR staticText[99];
			int len = SendMessage(hWnd, WM_GETTEXT,
				_ARRAYSIZE(staticText), (LPARAM)staticText);

			if (len == NULL)
				return FALSE;
			SelectObject(hdc, pData->font);
			SetBkColor(hdc, RGB(0x2C, 0x2C, 0x2C));


			if (pData->dwStyle & CS_TAB)
			{
				HPEN hp = CreatePen(PS_SOLID, 2, pData->BMT || pData->FOCUS ? RGB(2, 244, 235) : RGB(80, 80, 80));
				SelectObject(hdc, hp);
				RECT r = GetRect(hWnd);
				MoveToEx(hdc, r.left, r.bottom, 0);
				LineTo(hdc, r.left + 7, r.top);
				LineTo(hdc, r.right - 7, r.top);
				LineTo(hdc, r.right, r.bottom);
				LineTo(hdc, r.left, r.bottom);
				r.left += (r.right / 2);
				DrawTextC(r, hdc, staticText, len, 1);
			}
			else
				DrawTextC(r, hdc, staticText, len, 0);
		}
		else if (pData->dwStyle & CS_RECORD)
		{
			SetTextColor(hdc, pData->BMT || pData->FOCUS ? RGB(250, 24, 23) : RGB(200, 200, 200));
			SetBkColor(hdc, RGB(0x2C, 0x2C, 0x2C));
			WCHAR staticText[99];
			int len = SendMessage(hWnd, WM_GETTEXT,
				_ARRAYSIZE(staticText), (LPARAM)staticText);

			RECT r = GetRect(hWnd);
			SelectObject(hdc, pData->font);
			DrawTextC(r, hdc, staticText, len, 0);
		}
		else
		{

			HPEN hp = CreatePen(PS_SOLID, 2, pData->BMT || pData->FOCUS ? RGB(2, 244, 235) : RGB(200, 200, 200));
			SelectObject(hdc, hp);
			if (pData->dwStyle & CS_EXIT) {
				MoveToEx(hdc, 10, 10, 0);
				LineTo(hdc, r.right - 10, r.bottom - 10);
				MoveToEx(hdc, 10, r.bottom - 10, 0);
				LineTo(hdc, r.right - 10, r.top + 10);
			}
			else if (pData->dwStyle & CS_MINI) {
				MoveToEx(hdc, 10, r.bottom - 10, 0);
				LineTo(hdc, r.right - 10, r.bottom - 10);
			}
			else {
				MoveToEx(hdc, 10, 10, 0);
				LineTo(hdc, r.right - 10, 10);

				MoveToEx(hdc, 10, 9, 0);
				LineTo(hdc, 10, 20);

				MoveToEx(hdc, r.right - 10, 10, 0);
				LineTo(hdc, r.right - 10, 20);

				MoveToEx(hdc, 10, r.bottom - 10, 0);
				LineTo(hdc, r.right - 10, r.bottom - 10);
			}
		}
		//ReleaseDC(hWnd, hdc);
		RestoreDC(hdc, i);
		EndPaint(hWnd, &ps);
	}
	break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

LRESULT CALLBACK Theme::editProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) //edit box procedure.
{

	static BOOL OG = FALSE;

	//HWND hFocus =  GetFocus();
	CData* pData = (CData*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
	HWND focus = GetFocus();

	if (pData)
	{
		if (focus == hWnd)
			pData->FOCUS = TRUE;
		else
			pData->FOCUS = FALSE;
	}
	switch (message)
	{

	case WM_SETTEXT: {
		CWPSTRUCT* info = (CWPSTRUCT*)lParam;
		//if (wParam == 1) {
		wcscpy_s(pData->Text, (wchar_t*)&info->lParam);
		//}
	}
				   return DefWindowProc(hWnd, message, wParam, lParam);
				   //case WM_GETTEXT:
				   //	
				   //	return wsprintf((wchar_t*)&lParam, pData->Text);
				   //	//return DefWindowProc(hWnd, message, wParam, lParam);

	case WM_SETFOCUS:
	{
		// Create a solid black caret. 
		CreateCaret(hWnd, (HBITMAP)NULL, 1, pData->csize.bottom);

		// Adjust the caret position, in client coordinates. 
		SetCaretPos(pData->TxtDem.right, 0);

		// Display the caret. 
		ShowCaret(hWnd);
		InvalidateRect(hWnd, 0, TRUE);
	}
	break;
	case WM_KILLFOCUS:
		pData->FOCUS = FALSE;
		HideCaret(hWnd);
		pData->HighLight = FALSE;
		InvalidateRect(hWnd, 0, TRUE);
		break;
	case WM_LBUTTONDBLCLK:
	{
		pData->HighLight = !pData->HighLight;
		InvalidateRect(hWnd, 0, TRUE);
	}
	break;
	case WM_MOUSEACTIVATE:
		SetFocus(hWnd);
		break;
	case WM_KEYDOWN:
	{
		std::wstring wstr = pData->Text;
		if (pData->HighLight) {
			wstr.clear();
			pData->HighLight = FALSE;
		}

		if (wParam == VK_BACK) {
			if (wstr.length() >= 1)
				wstr.erase(wstr.length() - 1, 1);
		}

		else if (wParam != VK_SHIFT && wParam != VK_CAPITAL) {
			if (wParam == VK_OEM_PERIOD)
				wstr += '.';
			else
			{

				if (!GetKeyState(VK_CAPITAL) && !pData->ShiftState)
					wParam = tolower(wParam);
				wstr += (WCHAR)wParam;
			}
		}
		else if (wParam == VK_SHIFT) pData->ShiftState = TRUE;

		wcscpy_s(pData->Text, wstr.c_str());

		InvalidateRect(hWnd, 0, TRUE);
	}
	break;
	case WM_KEYUP:
	{
		if (wParam == VK_SHIFT)
			pData->ShiftState = FALSE;
	}
	break;
	case WM_MOUSEMOVE:
	{
		if (!pData->BMT)
		{
			TRACKMOUSEEVENT TME = { NULL };
			TME.cbSize = sizeof(TRACKMOUSEEVENT);
			TME.dwFlags = TME_LEAVE;
			TME.hwndTrack = hWnd;
			TME.dwHoverTime = NULL;
			pData->BMT = TrackMouseEvent(&TME);
			InvalidateRect(hWnd, 0, TRUE);
		}

	}
	break;
	case WM_MOUSELEAVE:
	{
		pData->BMT = FALSE;
		InvalidateRect(hWnd, 0, TRUE);
	}
	break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;

		HDC hdc = BeginPaint(hWnd, &ps);
		RECT r = GetRect(hWnd);
		HPEN hp;


		SelectObject(hdc, pData->font);
		//SetBkColor(hdc, pData->BMT || pData->FOCUS ? RGB(0x0F, 0xD3, 0xFF) : RGB(0x2C, 0x2C, 0x2C));
		SetBkColor(hdc, pData->HighLight ? RGB(0x2C, 0x4C, 0x6C) : RGB(0x2C, 0x2C, 0x2C));
		SetTextColor(hdc, RGB(200, 200, 200));

		if (pData->FOCUS)
		{

			int ws = wcsnlen(pData->Text, 99);/////////////////////
			pData->TxtDem = DrawTextC(r, hdc, (WCHAR*)pData->Text, ws, 0);////////////////
			SetCaretPos(pData->TxtDem.right, 0);
		}
		else
		{
			if (pData->Text[0] == '\0')///////////
			{
				wcscpy_s(pData->Text, pData->OGT);//////////////////////
			}
			int len = wcsnlen(pData->Text, sizeof(pData->Text) / sizeof(wchar_t));/////////////
			pData->TxtDem = DrawTextC(r, hdc, pData->Text, len, 0);///////////////

		}
		SetWindowText(hWnd, pData->Text);
		hp = CreatePen(PS_SOLID, 1, pData->BMT || pData->FOCUS ? RGB(2, 244, 235) : RGB(0x3B, 0x3B, 0x3B));
		SelectObject(hdc, hp);
		DrawEdge(hdc, pData->csize);
		EndPaint(hWnd, &ps);
	}
	break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}



void Theme::DrawEdge(HDC hdc, RECT& r)
{
	MoveToEx(hdc, 0, 0, 0);
	LineTo(hdc, r.right - 1, 0);
	MoveToEx(hdc, 0, r.bottom - 1, 0);
	LineTo(hdc, r.right - 1, r.bottom - 1);
	MoveToEx(hdc, 0, 0, 0);
	LineTo(hdc, 0, r.bottom - 1);
	MoveToEx(hdc, r.right - 1, 0, 0);
	LineTo(hdc, r.right - 1, r.bottom - 1);
}

HWND Theme::CWindow(RECT r, LPCWSTR wstr, LPCTSTR lpClassName, LPCTSTR lpWindowName, DWORD dwStyle, int x, int y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu)
{

	HWND hWnd = CreateWindow(lpClassName, lpWindowName, WS_CHILD, x, y, nWidth, nHeight, hWndParent, hMenu, (HINSTANCE)GetModuleHandle(NULL), nullptr);
	CData* pData = (CData*)malloc(sizeof(CData));
	if (hWnd == NULL || pData == NULL)
	{
		int er = GetLastError();
		return FALSE;
	}
	SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)pData);
	pData->hWnd = hWnd;
	pData->hMenu = (int)GetMenu(hWnd);
	pData->FOCUS = FALSE;
	pData->parent = hWndParent;
	pData->csize.right = nWidth;
	pData->csize.bottom = nHeight;
	pData->len = SendMessage(hWnd, WM_GETTEXT,
		_ARRAYSIZE(pData->OGT), (LPARAM)pData->OGT); //probably should just use lpWindowName.. oh well
	pData->ShiftState = FALSE;
	wcscpy_s(pData->Text, pData->OGT);
	pData->BMT = FALSE;
	pData->HighLight = FALSE;
	pData->font = CreateCFont(r, wstr);
	pData->dwStyle = dwStyle;
	if (dwStyle & WS_VISIBLE)
		ShowWindow(hWnd, SW_SHOW);
	return hWnd;
}


ATOM Theme::RClass(UINT style, WNDPROC wP, HINSTANCE hInstance, HCURSOR hCur, HICON hIcon, HBRUSH hbBG, LPCWSTR menuName, LPCWSTR className, int size)
{
	WNDCLASS wc;

	wc.style = style;
	wc.lpfnWndProc = wP;
	wc.hInstance = hInstance;
	wc.hCursor = hCur;
	wc.hIcon = hIcon;
	wc.hbrBackground = hbBG;
	wc.lpszMenuName = menuName;
	wc.lpszClassName = className;
	wc.cbClsExtra = size;
	wc.cbWndExtra = 0;


	return RegisterClass(&wc);
}

