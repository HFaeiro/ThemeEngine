
#include "ListView.h"
#include "locale"


class SortList
{
public:
	explicit SortList(int column, bool idec){}
	bool operator()(const std::vector<std::wstring>& cData1, const std::vector<std::wstring>& cData2)
	{
		if(dec)
		return std::lexicographical_compare(cData1[clumn].begin(), cData1[clumn].end(), cData2[clumn].begin(), cData2[clumn].end(), [](wchar_t ai, wchar_t bi) {
			
			return towlower(ai) > towlower(bi);
		});
		else
		{
			return std::lexicographical_compare(cData1[clumn].begin(), cData1[clumn].end(), cData2[clumn].begin(), cData2[clumn].end(), [](wchar_t ai, wchar_t bi) {

				return towlower(ai) < towlower(bi);
			});
		}
	}
private:
	int clumn;
	bool dec;

};

void InitializeListView(HINSTANCE hInstance, HBRUSH backgroundcolor)
{
	Theme::RClass(CS_HREDRAW | CS_VREDRAW, ListViewButtonProc, hInstance, LoadCursor(nullptr, IDC_ARROW), NULL,
	backgroundcolor, NULL, L"LVBUTTON", sizeof(Theme::ListViewButton));
Theme::RClass(CS_HREDRAW | CS_VREDRAW, (WNDPROC)ListViewProc, hInstance, LoadCursor(nullptr, IDC_ARROW), NULL,
	backgroundcolor, NULL, L"LV", sizeof(Theme::ListView));



}
bool compClientData(std::vector<std::wstring> cData1, std::vector<std::wstring> cData2)
{
	int retVal = 0;
	if (cData1.size() != cData2.size())
		return FALSE;
	for (int i = 0; i < cData1.size(); i++)
	{
		retVal += _wcsnicmp(cData1[i].c_str(), cData2[i].c_str(), 99);
	}
	
	return retVal;
}


static void
refreshButtons(HWND hWnd, Theme::ListView*& LV)
{
	
	int ActualScrollPosition = GetScrollPos(hWnd, SB_VERT);
	int tmpCounterScrollPosition = ActualScrollPosition;
	for (int i = LV->numOfPinned; i < LV->vLVButton.size(); i++)
	{

		for (int j = 0; j < LV->numOfPinned; j++)
		{
			if (tmpCounterScrollPosition >= LV->vListData.size())
				break;
			if (!compClientData(LV->vListData[tmpCounterScrollPosition], LV->vLVButton[j]->cData)) {
				tmpCounterScrollPosition++; // NEED TO WORK ON THIS TO FIX SCROLLING
			}
		}
		if (tmpCounterScrollPosition >= LV->vListData.size())
			break;
		//if(LV->vLVButton[i]->)
		LV->vLVButton[i]->cData = LV->vListData[tmpCounterScrollPosition];
		
		tmpCounterScrollPosition++;
	}
	RECT r = Theme::GetRect(hWnd);
	r.top = r.top + (LV->ButtonHeight * (LV->numOfPinned -1));
	InvalidateRect(hWnd, &r, TRUE);

}
Theme::ListViewButton*
addAndCreate(std::vector<std::wstring> cData, HWND parent, int y, int width, int height)
{
	HWND hWnd = CreateWindow(L"LVBUTTON", NULL, WS_CHILD, 0, y, width, height, parent, NULL, (HINSTANCE)GetModuleHandle(NULL), nullptr);
	if (!hWnd) {
		MessageBox(parent, L"LVBUTTON", L"FailedToCreateWindow", NULL);
	}
	Theme::ListViewButton* tmpLVButton = (Theme::ListViewButton*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
	tmpLVButton->cData = cData;
	return tmpLVButton;
}


LRESULT ListViewProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	Theme::ListView* LV = (Theme::ListView*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
	
	switch (message)
	{
	case IDB_REFRESH:
	case IDB_REMOVE:
	{
		if (wParam) {
			//std::vector<int>* invalidClients = (std::vector<int>*)(wParam);
			//std::vector<int> vInt;

			for (int i : *(std::vector<int>*)(wParam)) {
				if (i < LV->vListData.size()) {
					for (int j = 0; j < LV->vLVButton.size(); j++)
					{
						if (!compClientData(LV->vListData[i], LV->vLVButton[j]->cData)) {
							if(LV->vLVButton[j]->Pinned)
								SendMessage(hWnd, IDB_PIN, NULL, lParam);
							break;
						}
					}
				}
			}
			/*if ((*(std::vector<int>*)(wParam))[0] == 0)
			{
				HWND rHWND = (HWND)SendMessage(GetParent(hWnd), IDB_GETREMOTEHANDLE, NULL, NULL);
				LPSTREAM iStream = (LPSTREAM)SendMessage(rHWND, M_GETSTREAM, NULL, NULL);
				while (iStream != nullptr)
					Sleep(1);
			}*/
			int j = 0;
			int p = NULL;
			for (int i : *(std::vector<int>*)(wParam)) {
				p = i - j;
				if (p < 0) break;
				LV->vListData.erase(LV->vListData.begin() + p);
				j++;
			}

			SendMessage(hWnd, IDC_CLEAR, TRUE, NULL);


		}
		else
		{


			if (LV->vLVButton[lParam]->Pinned)
				SendMessage(hWnd, IDB_PIN, NULL, lParam);

			LV->vListData.erase(LV->vListData.begin() + lParam);
			SendMessage(hWnd, IDC_CLEAR, TRUE, NULL);



		}
		for (auto cData = LV->vListData.begin() + LV->numOfPinned; cData != LV->vListData.end(); cData++)
		{
			Theme::ListViewButton* tmpLVButton = addAndCreate(cData[0], hWnd, (LV->vLVButton.size()) * LV->ButtonHeight, LV->ButtonWidth, LV->ButtonHeight);
			tmpLVButton->ButtonNumber = LV->vLVButton.size();
			LV->vLVButton.push_back(tmpLVButton);
			ShowWindow(tmpLVButton->pData->hWnd, SW_SHOW);
		}
		//refreshButtons(hWnd, LV);
		

	}
	break;
	case IDR_GETLIST:
	{
		if(LV)
		return (LRESULT)&LV->vListData;
	}
	break;
	case IDC_CLEAR:
	{
		if (wParam != 1) {
			LV->vListData.erase(LV->vListData.begin(), LV->vListData.end());
			LV->numOfPinned = 0;
		}
		for (int i = (wParam == 1 ? LV->numOfPinned : 0 ); i < LV->vLVButton.size(); i++)
			DestroyWindow(LV->vLVButton[i]->pData->hWnd);
		LV->vLVButton.erase(LV->vLVButton.begin() + (wParam == 1 ? LV->numOfPinned : 0), LV->vLVButton.end());
		
		ShowScrollBar(hWnd, SB_VERT, 0);
		LV->VSCROLL = FALSE;
		InvalidateRect(hWnd, 0, TRUE);

	}
	break;
	case IDB_PIN:
	{

		if (!LV->vLVButton[lParam]->Pinned) {
			LV->vLVButton[LV->numOfPinned]->cData = LV->vLVButton[lParam]->cData; // swap pinned item to first slot after the last pinned item;
			LV->vLVButton[LV->numOfPinned]->Pinned = TRUE;

			LV->numOfPinned++; //to be pinned item was placed. so increase this counter/
			
		}
		else
		{
			if (lParam < LV->numOfPinned - 1)
				for (int i = lParam; i < LV->numOfPinned; i++)
					LV->vLVButton[i]->cData = LV->vLVButton[i+1]->cData;
			LV->numOfPinned--;
			LV->vLVButton[LV->numOfPinned]->Pinned = FALSE;
			
		}

		SCROLLINFO si;

		si.cbSize = sizeof(SCROLLINFO);
		si.fMask = SIF_RANGE;
		
		si.nMin = 0;
		si.nMax = ((LV->vListData.size() + LV->numOfPinned) - (LV->ControlHeight / LV->ButtonHeight));

		SetScrollInfo(hWnd, SB_VERT, &si, TRUE);
		refreshButtons(hWnd, LV);
		SetFocus(hWnd);
		break;
	}
	

	case WM_NCCREATE:
	{
		LV = new Theme::ListView();
		SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)LV);
		RECT r = Theme::GetRect(hWnd);
		LV->counter = 0; 
		LV->numOfPinned = 0;
		LV->ButtonHeight = 15;
		LV->ButtonWidth = r.right;
		LV->ControlHeight = r.bottom;
	}
	return TRUE;
	case WM_MOUSEWHEEL:
	{
		SendMessage(hWnd, WM_VSCROLL, WM_MOUSEWHEEL, GET_WHEEL_DELTA_WPARAM(wParam));
		return 0;
	}
	case WM_VSCROLL:
	{

				// Get all the vertical scroll bar information.

		SCROLLINFO si;
		si.cbSize = sizeof(si);
		si.fMask = SIF_ALL;
		GetScrollInfo(hWnd, SB_VERT, &si);

		// Save the position for comparison later on.
		int yPos = si.nPos;
		switch (LOWORD(wParam))
		{
		case WM_MOUSEWHEEL:
			si.nPos += (-lParam) / (LV->ControlHeight / LV->ButtonHeight);
			break;

			// User clicked the top arrow.
		case SB_LINEUP:
			si.nPos -= 1;
			break;

			// User clicked the bottom arrow.
		case SB_LINEDOWN:
			si.nPos += 1;
			break;

			// User clicked the scroll bar shaft above the scroll box.
		case SB_PAGEUP:
			si.nPos -= 3;
			break;

			// User clicked the scroll bar shaft below the scroll box.
		case SB_PAGEDOWN:
			si.nPos += 3;
			break;

			// User dragged the scroll box.
		case SB_THUMBTRACK:
			si.nPos = si.nTrackPos;
			break;

		default:
			break;
		}

		// Set the position and then retrieve it.  Due to adjustments
		// by Windows it may not be the same as the value set.
		si.fMask = SIF_POS;
		SetScrollInfo(hWnd, SB_VERT, &si, TRUE);
		GetScrollInfo(hWnd, SB_VERT, &si);

		// If the position has changed, scroll window and update it.
		if (si.nPos != yPos)
		{
			RECT r = Theme::GetRect(hWnd);
			r.top = r.top + (LV->ButtonHeight * LV->numOfPinned);
			ScrollWindowEx(hWnd, 0, (yPos - si.nPos) * 18,
				&r, &r, NULL, NULL, SW_ERASE | SW_INVALIDATE);
			//ScrollWindow(hWnd, 0, 18 * (yPos - si.nPos), NULL, NULL);
			//UpdateWindow(hWnd);
			refreshButtons(hWnd, LV);
			
		}

		return 0;
	}
	case IDB_ADD:
	{
		if (LV == NULL) {
			MessageBox(hWnd, L"LV == NULL", NULL, NULL);
			return FALSE;
		}

		std::vector<std::wstring> tmpClientData = *(std::vector<std::wstring>*)lParam;
		if ((LV->vLVButton.size() * LV->ButtonHeight) < LV->ControlHeight + 3) {
			Theme::ListViewButton* tmpLVButton = addAndCreate(tmpClientData, hWnd, (LV->vLVButton.size())*LV->ButtonHeight, LV->ButtonWidth, LV->ButtonHeight);
			tmpLVButton->ButtonNumber = LV->vLVButton.size();
			LV->vLVButton.push_back(tmpLVButton);
			ShowWindow(tmpLVButton->pData->hWnd, SW_SHOW);
		}
		LV->vListData.push_back(tmpClientData);

		
		SCROLLINFO si;

		si.cbSize = sizeof(SCROLLINFO);
		si.fMask = SIF_RANGE;
		si.nMin = 0;
		si.nMax = (LV->vListData.size() - (LV->ControlHeight / LV->ButtonHeight));

		SetScrollInfo(hWnd, SB_VERT, &si, TRUE);

		if ((LV->vListData.size() * LV->ButtonHeight) > LV->ControlHeight + 3 && !LV->VSCROLL) {
			ShowScrollBar(hWnd, SB_VERT, 1);
			LV->VSCROLL = TRUE;
		}
		else if ((LV->vListData.size() * LV->ButtonHeight) < LV->ControlHeight + 3 && LV->VSCROLL) {
			ShowScrollBar(hWnd, SB_VERT, 0);
			LV->VSCROLL = FALSE;
		}
		


	}
	break;
	case IDB_SORTDEC:

		std::sort(LV->vListData.begin(), LV->vListData.end(), SortList(lParam, TRUE));
		refreshButtons(hWnd, LV);
		break;
	
	case IDB_SORTACE:
		std::sort(LV->vListData.begin(), LV->vListData.end(),  SortList(lParam, FALSE));
		refreshButtons(hWnd, LV);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}


	return 0;
}



LRESULT __stdcall ListViewButtonProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static BOOL LBD = FALSE;

	Theme::ListViewButton* cData = (Theme::ListViewButton*)GetWindowLongPtr(hWnd, GWLP_USERDATA);

	HWND focus = GetFocus();

	if (cData)
	{
		if (focus == hWnd)
			cData->pData->FOCUS = TRUE;
		else
			cData->pData->FOCUS = FALSE;
	}

	switch (message)
	{
	case WM_NCCREATE:
	{
		Theme::ListViewButton* LVB = new Theme::ListViewButton();
		Theme::CData* pData = new Theme::CData();
		RECT rect = Theme::GetRect(hWnd);

		pData->hWnd = hWnd;
		pData->FOCUS = FALSE;

		pData->parent = GetParent(hWnd);
		pData->csize.right = rect.right;
		pData->csize.bottom = rect.bottom;
		pData->BMT = FALSE;
		pData->font = Theme::CreateCFont({ 14,0,0,0 }, L"Segoe UI");
		LVB->pData = pData;
		SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)LVB);
		return TRUE;
	}
	case GETFOCUS:
	{
		BOOL IB = (!cData->pData->FOCUS ? 0 : 1);
		return IB;
	}
	case WM_KILLFOCUS:
	{
		cData->pData->FOCUS = FALSE;
		InvalidateRect(hWnd, 0, TRUE);
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	case WM_ERASEBKGND:
	{
		if (cData->pData->BMT || cData->pData->FOCUS || cData->Pinned)
		{
			RECT rc;
			HDC hdc = (HDC)wParam;
			GetClientRect(hWnd, &rc);

			FillRect(hdc, &rc, CreateSolidBrush(cData->Pinned ? cData->pData->FOCUS ? RGB(0x6C, 0xC, 0x8C) : RGB(0x2C, 0x4C, 0x6C) : RGB(0x2C, 0x4C, 0x6C)));

			break;
		}
		else return DefWindowProc(hWnd, message, wParam, lParam);
	}

	case WM_MOUSEACTIVATE:
	{
		if (!cData->pData->FOCUS)
			SetFocus(hWnd);
		else
			SendMessage(hWnd, WM_KILLFOCUS, 0, 0);
		InvalidateRect(hWnd, 0, TRUE);

	}
	break;
	case WM_COMMAND:
	{
		switch (LOWORD(wParam)) {
		case LV_REFRESH:
			SendMessage(GetParent(hWnd), IDB_REFRESH, 0, cData->ButtonNumber);
			break;
		case LV_REMOVE:
			SendMessage(GetParent(hWnd),IDB_REMOVE, 0, cData->ButtonNumber);
			break;
		case LV_PIN:
			SendMessage(GetParent(hWnd), IDB_PIN, 0, cData->ButtonNumber);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	}
	case WM_RBUTTONDOWN:
	{
		POINT cursor = { 0 };
		GetCursorPos(&cursor);
		HMENU popupMenu = CreatePopupMenu();
		InsertMenu(popupMenu, NULL, MF_STRING, LV_PIN, cData->Pinned ? L"UnPin" : L"Pin");
		InsertMenu(popupMenu, NULL, MF_STRING, LV_REMOVE, L"Remove");
		TrackPopupMenu(popupMenu, TPM_LEFTALIGN, cursor.x, cursor.y, 0, hWnd, NULL);


		break;
	}
	case WM_MOUSEMOVE:
	{
		if (!cData->pData->BMT)
		{
			TRACKMOUSEEVENT TME = { NULL };
			TME.cbSize = sizeof(TRACKMOUSEEVENT);
			TME.dwFlags = TME_LEAVE;
			TME.hwndTrack = hWnd;
			TME.dwHoverTime = NULL;
			cData->pData->BMT = TrackMouseEvent(&TME);
			InvalidateRect(hWnd, 0, TRUE);
		}
		//


		break;
	}
	case WM_MOUSELEAVE:
	{
		cData->pData->BMT = FALSE; //hover == false
		InvalidateRect(hWnd, 0, TRUE);

	}
	break;
	case WM_PAINT:
	{
		if (cData->pData == NULL)
			return FALSE;
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		RECT r = Theme::GetRect(hWnd);
		HPEN hp;


		int menu = (int)GetMenu(hWnd);
		SetTextColor(hdc, RGB(200, 200, 200));

		SelectObject(hdc, cData->pData->font);


		SetBkColor(hdc, cData->pData->BMT || cData->pData->FOCUS || cData->Pinned ? cData->Pinned ? cData->pData->FOCUS ? RGB(0x6C, 0xC, 0x8C) : RGB(0x2C, 0x4C, 0x6C) : RGB(0x2C, 0x4C, 0x6C) : RGB(0x2C, 0x2C, 0x2C));

		int size = cData->cData.size();
		RECT tmpr = Theme::DrawTextC(r, hdc, &cData->cData[0][0], wcslen(&cData->cData[0][0]), 0);
		int spacing = (r.right - (tmpr.right / 2)) / size;
		r.left += spacing + (tmpr.right / 2);
		for (int i = 1; i < size; i++)
		{

			Theme::DrawTextC(r, hdc, &cData->cData[i][0], wcslen(&cData->cData[i][0]), 1);
			r.left += spacing;
		}


		EndPaint(hWnd, &ps);
	}
	break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}