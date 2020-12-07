#include "stdafx.h"
#include "TTEdit.h"
CTTEdit::CTTEdit() {

}

CTTEdit::~CTTEdit() {

}

void CTTEdit::OnMouseMove(UINT nFlags, CPoint point)
{
	// However, the combobox may be captured thus we must know when the mouse cursor
	// out of the area of the combobox, and send 'WM_MOUSELEAVE' notification.
	CRect rcBounds;
	GetClientRect(&rcBounds);
	if (rcBounds.PtInRect(point) == FALSE)
	{
		SendMessage(WM_MOUSELEAVE);
	}
	CRect rect(point.x - 200, point.y - 200, point.x + 200, point.y + 200);
	RedrawWindow(&rect);

	//RedrawWindow();
	CClientDC dc(this);
	dc.TextOut(point.x, point.y, _T("文本提示"));
	// Here is tracking mouse event such as 'HOVER', 'LEAVE'.
	//if (m_isEnter == FALSE)
	//{
	//	OnTrackMouseEvent(m_hWnd, TME_HOVER | TME_LEAVE);
	//	m_isEnter = TRUE;
	//}

	CEdit::OnMouseMove(nFlags, point);
}
