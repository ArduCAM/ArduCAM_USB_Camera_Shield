// TTComboBox.cpp : implementation file
//

#include "stdafx.h"
#include "TTComboBox.h"

const int CTTComboBox::alignLeft = 0;
const int CTTComboBox::alignRight = 1;
const int CTTComboBox::alignMiddle = 2;

CWnd	CTTComboBox::m_tipWnd;
int		CTTComboBox::m_OriginalSel = LB_ERR;
UINT	CTTComboBox::m_nDelayTime = 9000; // millisecond
BOOL	CTTComboBox::m_isEnter = FALSE;
CFont	CTTComboBox::m_font;
CMap<HWND, HWND, WNDPROC, WNDPROC&> CTTComboBox::m_mapWndProc;
CMap<HWND, HWND, CTTComboBox*, CTTComboBox*> CTTComboBox::m_mapCombo;
/////////////////////////////////////////////////////////////////////////////
// CTTComboBox

CTTComboBox::CTTComboBox()
{
	// Using system tool-tip font for the tool-tip window
	NONCLIENTMETRICS ncm = {0};
	ncm.cbSize = sizeof(NONCLIENTMETRICS);
	SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &ncm, 0);
	if (m_font.m_hObject == NULL)
		m_font.CreateFontIndirect(&ncm.lfStatusFont);

	m_rcButton = CRect(0,0,0,0);
	m_isEnableTool = TRUE; // The tool-tip is allowed

	m_alignStyle = CTTComboBox::alignLeft;

	// Create the tool-tip window
	this->CreateTooltipWnd();
}

CTTComboBox::~CTTComboBox()
{
	// Clean up the map 
	if (m_cbi.hwndList)
	{
		m_mapWndProc.RemoveKey(m_cbi.hwndList);
		m_mapCombo.RemoveKey(m_cbi.hwndList);
	}
	if (m_cbi.hwndItem)
	{
		m_mapWndProc.RemoveKey(m_cbi.hwndItem);
		m_mapCombo.RemoveKey(m_cbi.hwndItem);
	}
	// Destroy the tool-tip window
	DestroyTooltipWnd();

	// Remove all disabled items
	m_arrDisabledItems.RemoveAll();
}

CEdit* CTTComboBox::GetEditBox()
{
	if (::IsWindow(m_cbi.hwndItem))
		return (CEdit*)CWnd::FromHandle(m_cbi.hwndItem);

	return NULL;
}

CListBox* CTTComboBox::GetListBox()
{
	if (::IsWindow(m_cbi.hwndList))
		return (CListBox*)CWnd::FromHandle(m_cbi.hwndList);

	return NULL;
}

// Default, Align the left-edge of list-box with the combo,
// also we can set other alignment style here.
void CTTComboBox::SetAlignStyle(int style)
{
	m_alignStyle = style;
}

void CTTComboBox::SetTooltipDelay(UINT nDelay)
{
	m_nDelayTime = nDelay;
}

UINT CTTComboBox::GetTooltipDelay()
{
	return m_nDelayTime;
}

void CTTComboBox::SetToolTip(BOOL bEnable/*=TRUE*/)
{
	m_isEnableTool = bEnable;
}

void CTTComboBox::SetTipText(int nIndex, LPCTSTR pszTipText)
{
	CListBox* pListbox = this->GetListBox();
	if (!pListbox) return;

	if (pszTipText)
		m_mapTipTexts.SetAt(nIndex, pszTipText);
	else
		m_mapTipTexts.RemoveKey(nIndex);
}

BOOL CTTComboBox::SetDisabledItem(int nIndex, BOOL bDisabled/*=TRUE*/)
{
	if (nIndex < 0 || nIndex > (this->GetCount() - 1))
		return FALSE;
	
	for (int i = 0; i < m_arrDisabledItems.GetSize(); i++)
	{
		if ((UINT)nIndex == m_arrDisabledItems.GetAt(i))
		{
			if (bDisabled == FALSE)
			{
				m_arrDisabledItems.RemoveAt(i);
			}

			return TRUE;
		}
	}

	m_arrDisabledItems.Add(nIndex);

	return TRUE;
}

BEGIN_MESSAGE_MAP(CTTComboBox, CComboBox)
	//{{AFX_MSG_MAP(CTTComboBox)
	ON_WM_CREATE()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_CONTROL_REFLECT(CBN_DROPDOWN, OnDropdown)
	ON_WM_MOUSEWHEEL()
	//}}AFX_MSG_MAP
	ON_WM_DESTROY()
	ON_MESSAGE(WM_MOUSEHOVER, OnMouseOver)
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTTComboBox message handlers

int CTTComboBox::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CComboBox::OnCreate(lpCreateStruct) == -1)
		return -1;

	// If the combo-box isn't dynamically subclassed that 
	// should get rid of the /* and */ commenting characters below
	/*
	if (m_isEnableTool == TRUE)
	{
		InstallHookForListboxAndEditbox();
	}
	*/
	return 0;
}

void CTTComboBox::CreateTooltipWnd()
{
	// However, the tool-tip window only once created and all combobox 
	// have share in the tool-tip window.
	if (!::IsWindow(m_tipWnd.m_hWnd))
	{
		m_tipWnd.CreateEx(WS_EX_TOOLWINDOW,
			AfxRegisterWndClass(0, AfxGetApp()->LoadStandardCursor(IDC_ARROW)),
		NULL, WS_POPUP, 0, 0, 0, 0, NULL, NULL);

		// Set up hooking with the tip window
		// Note: Refer to MSDN, In Windows NT/2000, You cannot change this attribute(GWL_WNDPROC) 
		// if the window(i.e, too-tip) does not belong to the same process as the calling thread.
		WNDPROC oldTipWndProc = (WNDPROC)::SetWindowLongPtr(m_tipWnd.m_hWnd, 
			GWLP_WNDPROC, (DWORD_PTR)HookTipWndProc);
		// Add the old hook(i.e, hook associated with window) to the map
		m_mapWndProc.SetAt(m_tipWnd.m_hWnd, oldTipWndProc);
	}
}

void CTTComboBox::DestroyTooltipWnd()
{
	INT_PTR nSize = m_mapWndProc.GetCount();
	WNDPROC oldTipWndProc;
	HWND hWnd = m_tipWnd.m_hWnd;
	BOOL bRet = m_mapWndProc.Lookup(hWnd, oldTipWndProc);
	if (nSize == 1 && bRet == TRUE)
	{// Reset only one window procedure(i.e, the tool-tip window procedure)
	 // that means any combobox doesn't use the tool-tip window. 
		m_tipWnd.DestroyWindow();
		m_mapWndProc.RemoveKey(hWnd);
		m_mapWndProc.RemoveAll();
		m_mapCombo.RemoveAll();
	}
}

void CTTComboBox::InstallHookForListboxAndEditbox()
{
	// Get the list-box/edit-box within the combobox and set up hooking with it.
	// Note: this only for Win98 or WinNT5.0 or later, 
	// However you must defined WINVER >= 0x0500 within the project
	ZeroMemory(&m_cbi, sizeof(COMBOBOXINFO));
	m_cbi.cbSize = sizeof(COMBOBOXINFO);
	::GetComboBoxInfo(m_hWnd, &m_cbi);
	m_rcButton = m_cbi.rcButton;

	if (m_cbi.hwndList)
	{
		// Set up hooking for the list-box within the combobox
		// Note: Refer to MSDN, In Windows NT/2000, You cannot change this attribute(GWL_WNDPROC) 
		// if the window(i.e, list-box) does not belong to the same process as the calling thread.
		WNDPROC oldListWndProc = (WNDPROC)::SetWindowLongPtr(m_cbi.hwndList, GWLP_WNDPROC, (DWORD_PTR)HookListboxWndProc);
		// Add the old hook(i.e, hook associated with window) to the map
		m_mapWndProc.SetAt(m_cbi.hwndList, oldListWndProc);
		m_mapCombo.SetAt(m_cbi.hwndList, (CTTComboBox*)this);
	}
	
	if (m_cbi.hwndItem)
	{
		// Set up hooking for the edit-box within the combobox
		// Note: Refer to MSDN, In Windows NT/2000, You cannot change this attribute(GWL_WNDPROC) 
		// if the window(i.e, edit-box) does not belong to the same process as the calling thread.
		WNDPROC oldEditWndProc = (WNDPROC)::SetWindowLongPtr(m_cbi.hwndItem, GWLP_WNDPROC, (DWORD_PTR)HookEditboxWndProc);
		// Add the old hook(i.e, hook associated with window) to the map
		m_mapWndProc.SetAt(m_cbi.hwndItem, oldEditWndProc);
		m_mapCombo.SetAt(m_cbi.hwndItem, (CTTComboBox*)this);
	}
}

// The callback function that hooks up and processes messages
// send to the list-box within the combobox
LRESULT CALLBACK 
CTTComboBox::HookListboxWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	CListBox* pList = (CListBox*)CWnd::FromHandle(hWnd);
	CTTComboBox* pThisCombo = NULL;
	m_mapCombo.Lookup(hWnd, pThisCombo);

	if (message == WM_MOUSEMOVE)
	{
		WORD xPos, yPos;
		xPos = LOWORD(lParam);
		yPos = HIWORD(lParam);
		CPoint point(xPos, yPos); 
		CRect rcClient;
		::GetClientRect(hWnd, &rcClient);
		if (rcClient.PtInRect(point))
		{// Handle mouse move event that may show a tool-tip window...
			CTTComboBox::HandleListboxMouseMove(pList, wParam, point);
		}
		else
		{// However, the list-box may be captured thus we must know when the mouse cursor
		 // out of the area of the list-box, and send 'WM_MOUSELEAVE' notification.
			::SendMessage(hWnd, WM_MOUSELEAVE, wParam, lParam);
		}

		if (!m_isEnter)
		{// Tracking the mouse event which are hovering and leaving.
			OnTrackMouseEvent(hWnd, TME_HOVER|TME_LEAVE);
			m_isEnter = TRUE;
		}
	}
	else if (message == WM_MOUSELEAVE)
	{
		// When the mouse cursor has been left current window, the original select of the list-box 
		// to reset LB_ERR.
		m_OriginalSel = LB_ERR;
		m_isEnter = FALSE;	
		// The tool-tip window is hidden
		m_tipWnd.ShowWindow(SW_HIDE);
	}
	else if (message == WM_CAPTURECHANGED)
	{	// Ignore the mouse capture changed...
		return 1;
	}
	else if (message == WM_LBUTTONDOWN || 
			 message == WM_LBUTTONDBLCLK)
	{
		WORD xPos, yPos;
		xPos = LOWORD(lParam);
		yPos = HIWORD(lParam);
		CPoint point(xPos, yPos); 
		CRect rcClient;
		::GetClientRect(hWnd, &rcClient);
		if (rcClient.PtInRect(point))
		{
			BOOL bOutside;
			int curSel = pList->ItemFromPoint(point, bOutside);
			if (!bOutside && curSel != LB_ERR)
			{
				for (int i = 0; i < pThisCombo->m_arrDisabledItems.GetSize(); i++)
				{
					if ((UINT)curSel == pThisCombo->m_arrDisabledItems.GetAt(i))
					{
						return TRUE; // Don't process the message
					}
				}
			}
		}
	}
	else if (message == WM_SHOWWINDOW)
	{
		if (wParam == TRUE) // Show up!
		{
			pThisCombo->AlignListBoxWithCombo();
		}
	}

	// Get previous window procedure
	WNDPROC oldListWndProc;
	m_mapWndProc.Lookup(hWnd, oldListWndProc);
	// Call previous window procedure
	return ::CallWindowProc(oldListWndProc, hWnd, message, wParam, lParam);
}

LRESULT CALLBACK 
CTTComboBox::HookEditboxWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{	
	CWnd* pWnd = CWnd::FromHandle(hWnd);
	
	if (message == WM_MOUSEMOVE)
	{
		// However, the edit-box may be captured thus we must know when the mouse cursor
		// out of the area of the edit-box, and send 'WM_MOUSELEAVE' notification.
		CPoint point;
		point.x = LOWORD(lParam);
		point.y = HIWORD(lParam);
		CRect rcBounds;
		::GetClientRect(hWnd, &rcBounds);
		if (rcBounds.PtInRect(point) == FALSE)
		{
			::SendMessage(hWnd, WM_MOUSELEAVE, wParam, lParam);
		}

		if (!m_isEnter)
		{// Tracking the mouse event which are hovering and leaving.
			OnTrackMouseEvent(hWnd, TME_HOVER|TME_LEAVE);
			m_isEnter = TRUE;
		}
	}
	else if (message == WM_MOUSEHOVER)
	{
		HandleEditboxMouseOver(pWnd);
	}
	else if (message == WM_MOUSELEAVE)
	{
		// When the mouse cursor has been left current window, the original select of the list-box 
		// to reset LB_ERR.
		m_OriginalSel = LB_ERR;
		m_isEnter = FALSE;
		// The tool-tip window is hidden
		m_tipWnd.ShowWindow(SW_HIDE);
	}

	// Get previous window procedure
	WNDPROC oldEditWndProc;
	m_mapWndProc.Lookup(hWnd, oldEditWndProc);
	// Call previous window procedure
	return ::CallWindowProc(oldEditWndProc, hWnd, message, wParam, lParam);
}

// The callback function that hooks up and processes messages
// send to the tool-tip window.
LRESULT CALLBACK CTTComboBox::HookTipWndProc(HWND hWnd, UINT message, WPARAM wp, LPARAM lp)
{
	if (message == WM_PAINT)
	{
		CTTComboBox::HandleOnPaint();
	}
	else if (message == WM_TIMER)
	{
		CTTComboBox::HandleOnTimer(wp);
	}
	else if (message == WM_SHOWWINDOW &&
		wp == FALSE)
	{
		 // Release the mouse capture from a window 
		 // mean the mouse will reenter the(or other) window at next time
		ReleaseCapture();
	}

	// Get previous window procedure
	WNDPROC oldTipWndProc;
	m_mapWndProc.Lookup(hWnd, oldTipWndProc);

	// Call previous window procedure
	return ::CallWindowProc(oldTipWndProc, hWnd, message, wp, lp);
}
#include <shlobj.h>
#include <shellapi.h>

void CTTComboBox::HandleListboxMouseMove(CListBox* pList, UINT uFlag, CPoint point)
{	
	BOOL bOutside;
	int curSel = pList->ItemFromPoint(point, bOutside);
	if (bOutside || curSel == LB_ERR ||
		curSel < 0 || curSel >= pList->GetCount())
		return; // the point is outside the client area of the item in the list-box

	if (m_OriginalSel == curSel)
	{// If The current option is equal to original that get outta here
		return; 
	}

	m_OriginalSel = curSel;

	CDC* pDC = m_tipWnd.GetDC();
	CFont* pOldFont = pDC->SelectObject(&m_font);
	CString strText;

	BOOL bForceAppear = FALSE;
	CTTComboBox* pThisCombo = NULL;
	m_mapCombo.Lookup(pList->m_hWnd, pThisCombo);
	if (pThisCombo)
	{
		if (pThisCombo->m_mapTipTexts.Lookup(curSel, strText))
			bForceAppear = TRUE;
	}
	if (strText.IsEmpty())
		pList->GetText(curSel, strText);

	CRect rcBounds;
	pList->GetItemRect(curSel, &rcBounds);
	CRect rcDraw = rcBounds;
	pDC->DrawText(strText, &rcDraw, DT_CALCRECT|DT_SINGLELINE|DT_CENTER|DT_VCENTER|DT_NOPREFIX);
	pDC->SelectObject(pOldFont);
	::ReleaseDC(m_tipWnd.GetSafeHwnd(), pDC->m_hDC);

	if (!bForceAppear && 
		(rcDraw.right <= rcBounds.right))
	{// However don't show the tool-tip window and get outta here, 
	 // unless the tool-tip window's width is greater than 
	 // the option's width in the list-box.
		m_tipWnd.ShowWindow(SW_HIDE);
		return;
	}

	rcDraw.InflateRect(2,2);
	pList->ClientToScreen(&rcDraw);
	m_tipWnd.SetWindowText(strText);

	m_tipWnd.ShowWindow(SW_HIDE); // Last showing window is hidden

	// Capture current window that prevent mouse cursor leave current window,
	// The cause is the mouse cursor will be over the tool-tip window(see below).
	if (GetCapture() != pList)
		::SetCapture(pList->m_hWnd);

	// Delayed to show the tool-tip window by given position
	m_tipWnd.SetWindowPos(&CWnd::wndTopMost, 
		rcDraw.left, rcDraw.top, rcDraw.Width(), rcDraw.Height(), SWP_NOACTIVATE | SWP_SHOWWINDOW);
	m_tipWnd.SetTimer(1, m_nDelayTime, NULL);
}

void CTTComboBox::HandleEditboxMouseOver(CWnd* pWnd)
{
	CRect rcBounds;
	pWnd->GetClientRect(&rcBounds);

	CString strText;
	pWnd->GetWindowText(strText);

	CDC* pDC = m_tipWnd.GetDC();
	CFont* pOldFont = pDC->SelectObject(&m_font);
	CRect rcDraw = rcBounds;
	pDC->DrawText(strText, &rcDraw, DT_CALCRECT|DT_SINGLELINE|DT_CENTER|DT_VCENTER|DT_NOPREFIX);
	pDC->SelectObject(pOldFont);
	::ReleaseDC(m_tipWnd.GetSafeHwnd(), pDC->m_hDC);

	if (rcDraw.right <= rcBounds.right)
	{// However don't show the tool-tip window and get outta here, 
	 // unless the tool-tip window's width is greater than 
	 // the option's width in the list-box.
		m_tipWnd.ShowWindow(SW_HIDE);
	}
	else
	{
		rcDraw.bottom = rcBounds.bottom;
		rcDraw.InflateRect(2,2);
		pWnd->ClientToScreen(&rcDraw);
		m_tipWnd.SetWindowText(strText);
		m_tipWnd.ShowWindow(SW_HIDE); // Last showing window is hidden
		
		// Capture current window that prevent mouse cursor leave current window,
		// The cause is the mouse cursor will be over the tool-tip window(see below).
		::SetCapture(pWnd->m_hWnd);

		// Delayed to show the tool-tip window by given position
		m_tipWnd.SetWindowPos(&CWnd::wndTopMost, 
			rcDraw.left, rcDraw.top, rcDraw.Width(), rcDraw.Height(), SWP_NOACTIVATE | SWP_SHOWWINDOW);
		m_tipWnd.SetTimer(1, m_nDelayTime, NULL);
	}
}

// Handler for repaint tool-tip window
void CTTComboBox::HandleOnPaint()
{
	CPaintDC dc(&m_tipWnd);
	CRect rc;
	m_tipWnd.GetClientRect(&rc);
	CBrush brush(GetSysColor(COLOR_INFOBK));
	dc.FillRect(&rc, &brush);

	CBrush border(RGB(0,0,0));
	dc.FrameRect(&rc, &border);

	CString strText;
	m_tipWnd.GetWindowText(strText);
	dc.SetBkMode(TRANSPARENT);
	CFont* pOldFont = dc.SelectObject(&m_font);
	dc.DrawText(strText, &rc, DT_SINGLELINE|DT_CENTER|DT_VCENTER|DT_NOPREFIX);

	dc.SelectObject(pOldFont);
}

// Handler for each interval specified when we used SetTimer to install a timer
// for tool-tip window.
void CTTComboBox::HandleOnTimer(UINT nIDEvent)
{
	m_tipWnd.KillTimer(nIDEvent);
	m_tipWnd.ShowWindow(SW_HIDE);
}

BOOL CTTComboBox::OnTrackMouseEvent(HWND hWnd, DWORD dwFlags)
{
	TRACKMOUSEEVENT tme;
	tme.cbSize = sizeof(TRACKMOUSEEVENT);
	tme.dwFlags = dwFlags;
	tme.dwHoverTime = HOVER_DEFAULT;
	tme.hwndTrack = hWnd;
	return ::_TrackMouseEvent(&tme);
}

void CTTComboBox::AlignListBoxWithCombo()
{
	if (m_cbi.hwndList == NULL ||
		!::IsWindow(m_cbi.hwndList))
		return;

	CListBox* pList = (CListBox*)CWnd::FromHandle(m_cbi.hwndList);
	CRect l_rcCombo;
	GetClientRect(&l_rcCombo);
	CRect l_rcList;
	pList->GetClientRect(&l_rcList);
	int dropWidth = GetDroppedWidth();
	int listWidth = l_rcList.Width();
	int xOffset = 0;
	switch (m_alignStyle)
	{
	case 0:
		xOffset = 0;
		break;
	case 1:
		xOffset = dropWidth - l_rcCombo.Width();
		break;
	case 2:
		xOffset = (dropWidth - l_rcCombo.Width()) / 2;
		break;
	default:
		xOffset = 0;
		break;
	}
	pList->ClientToScreen(&l_rcList);
	pList->SetWindowPos(NULL, l_rcList.left - xOffset, l_rcList.top, 0, 0, SWP_NOSIZE);
}

void CTTComboBox::PreSubclassWindow() 
{
	CComboBox::PreSubclassWindow();

	// If the combo-box isn't dynamically subclassed that should get rid of 
	// the codes as follows and the codes should be placed in the OnCreate function.
	if (m_isEnableTool == TRUE)
	{
		InstallHookForListboxAndEditbox();
	}
}

void CTTComboBox::OnMouseMove(UINT nFlags, CPoint point) 
{
	// However, the combobox may be captured thus we must know when the mouse cursor
	// out of the area of the combobox, and send 'WM_MOUSELEAVE' notification.
	CRect rcBounds;
	GetClientRect(&rcBounds);
	if (rcBounds.PtInRect(point) == FALSE)
	{
		SendMessage(WM_MOUSELEAVE);
	}
	
	// Here is tracking mouse event such as 'HOVER', 'LEAVE'.
	if (m_isEnter == FALSE)
	{
		OnTrackMouseEvent(m_hWnd, TME_HOVER|TME_LEAVE);
		m_isEnter = TRUE;
	}

	CComboBox::OnMouseMove(nFlags, point);
}

LRESULT CTTComboBox::OnMouseOver(WPARAM wp, LPARAM lp) 
{
	if (m_isEnableTool == FALSE)
		return 0;

	// The below code is obsolete, 
	// Ideally, the below code is to block mouse over Editbox for DropDown style.
	// Somehow, The m_cbi.hwndItem is also non-zero for a DropList under Vista common controls,
	// or the project is compiled under Unicode version?.
	
	// Actually, if The combo is DropDown style, it has owns Editbox, 
	// and the event function won't be fired when mouse hover on that Editbox.
	// Thus, the below code should be useless, I decided to remove the below code.
	/* OBSOLETE..
	if (::IsWindow(m_cbi.hwndItem))
		return 0; // If the edit-box has exist that gotta here
	*/

	CRect rcBounds;
	GetClientRect(&rcBounds);
	rcBounds.DeflateRect(2, 2);
	rcBounds.right -= m_rcButton.Width();

	CString strText;
	BOOL bForceAppear = FALSE;
	if (m_mapTipTexts.Lookup(GetCurSel(), strText))
		bForceAppear = TRUE;
	if (strText.IsEmpty())
		GetWindowText(strText);

	CDC* pDC = m_tipWnd.GetDC();
	CFont* pOldFont = pDC->SelectObject(&m_font);
	CRect rcDraw = rcBounds;
	pDC->DrawText(strText, &rcDraw, DT_CALCRECT|DT_SINGLELINE|DT_CENTER|DT_VCENTER|DT_NOPREFIX);
	pDC->SelectObject(pOldFont);
	::ReleaseDC(m_tipWnd.GetSafeHwnd(), pDC->m_hDC);

	if (!bForceAppear &&
		rcDraw.right <= rcBounds.right)
	{// However don't show the tool-tip window and get outta here, 
	 // unless the tool-tip window's width is greater than 
	 // the option's width in the list-box.
		m_tipWnd.ShowWindow(SW_HIDE);
		return 0;
	}

	rcDraw.bottom = rcBounds.bottom;
	rcDraw.InflateRect(2,2);
	ClientToScreen(&rcDraw);
	m_tipWnd.SetWindowText(strText);
	m_tipWnd.ShowWindow(SW_HIDE); // Last showing window is hidden

	// Capture current window that prevent mouse cursor leave current window,
	// The cause is the mouse cursor will be over the tool-tip window(see below).
	SetCapture();

	// Delayed to show the tool-tip window by given position
	m_tipWnd.SetWindowPos(&CWnd::wndTopMost, 
		rcDraw.left, rcDraw.top, rcDraw.Width(), rcDraw.Height(), SWP_NOACTIVATE | SWP_SHOWWINDOW);
	m_tipWnd.SetTimer(1, m_nDelayTime, NULL);

	return 0;
}

LRESULT CTTComboBox::OnMouseLeave(WPARAM wp, LPARAM lp)
{
	// The mouse cursor has leave current window
	m_isEnter = FALSE;
	// The tool-tip window is hidden
	m_tipWnd.ShowWindow(SW_HIDE);

	return 0;
}

void CTTComboBox::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (m_tipWnd.IsWindowVisible())
		ReleaseCapture();
	
	CComboBox::OnLButtonDown(nFlags, point);
}

void CTTComboBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	CRect rc;
	GetWindowRect(&rc);
	rc.bottom -= 15;
	ASSERT(lpDrawItemStruct->CtlType == ODT_COMBOBOX);
	CString strText;
	// If there are no list box items, skip this message. 
	if ((int)lpDrawItemStruct->itemID < 0 || (int)lpDrawItemStruct->itemID > (GetCount()-1) )
		return;
	this->GetLBText(lpDrawItemStruct->itemID, strText);
	CDC dc;

	dc.Attach(lpDrawItemStruct->hDC);

	// Save these value to restore them when done drawing.
	COLORREF crOldTextColor = dc.GetTextColor();
	COLORREF crOldBkColor = dc.GetBkColor();

	// The item maybe disabled...
	BOOL bDisabledItem = FALSE;
	for (int i = 0; i < m_arrDisabledItems.GetSize(); i++)
	{
		if (lpDrawItemStruct->itemID == m_arrDisabledItems.GetAt(i))
		{
			bDisabledItem = TRUE;
			break;
		}
	}   

	// If this item is selected, set the background color 
	// and the text color to appropriate values. Erase
	// the rect by filling it with the background color.
	if ((lpDrawItemStruct->itemAction | ODA_SELECT) &&
	  (lpDrawItemStruct->itemState  & ODS_SELECTED) && bDisabledItem == FALSE)
	{
	  dc.SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
	  dc.SetBkColor(::GetSysColor(COLOR_HIGHLIGHT));
	  dc.FillSolidRect(&lpDrawItemStruct->rcItem, ::GetSysColor(COLOR_HIGHLIGHT));
	}
	else
	{
		dc.FillSolidRect(&lpDrawItemStruct->rcItem, crOldBkColor);
	}

	if (lpDrawItemStruct->itemState & ODS_DISABLED)
	{
		dc.SetTextColor(::GetSysColor(COLOR_GRAYTEXT));
	}

	// Draw the text.
	dc.DrawText(
	  strText,
	  &lpDrawItemStruct->rcItem,
	  DT_LEFT|DT_SINGLELINE|DT_VCENTER|DT_NOPREFIX);

	// Reset the background color and the text color back to their
	// original values.
	dc.SetTextColor(crOldTextColor);
	dc.SetBkColor(crOldBkColor);

	dc.Detach();
}

void CTTComboBox::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
//	lpMeasureItemStruct->itemHeight = 63;
}

BOOL CTTComboBox::PreCreateWindow(CREATESTRUCT& cs) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CComboBox::PreCreateWindow(cs);
}

LRESULT CTTComboBox::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	return CComboBox::WindowProc(message, wParam, lParam);
}

void CTTComboBox::OnDestroy()
{
	CComboBox::OnDestroy();
}

void CTTComboBox::OnDropdown() 
{
}

BOOL CTTComboBox::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	BOOL ret = CComboBox::OnMouseWheel(nFlags, zDelta, pt);

	BOOL bSelBack = zDelta >= 0 ? TRUE : FALSE;
	int nCurSel = GetCurSel();
	if (nCurSel != CB_ERR)
	{
		for (int i = 0; i < m_arrDisabledItems.GetSize(); i++)
		{
			int nItem = m_arrDisabledItems.GetAt(i);
			if (nItem == nCurSel)
			{
				if (bSelBack == TRUE)
					--nCurSel;
				else
					++nCurSel;
				
				SetCurSel(nCurSel);
			}
		}
	}

	return ret;
}

