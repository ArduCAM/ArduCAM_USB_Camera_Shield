#pragma once
#ifndef __TT_EDIT__
#define __TT_EDIT__

#include <afxwin.h>
#include <afxtempl.h>

/////////////////////////////////////////////////////////////////////////////
// CTTComboBox window
class CTTEdit : public CEdit
{
	// Construction
public:
	CTTEdit();

// Implementation
public:
	virtual ~CTTEdit();

private:
	// Pointer to the tool-tip window for the list-box
	static CWnd		m_tipWnd;
	// Generated message map functions
protected:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};

/////////////////////////////////////////////////////////////////////////////

#endif // __TT_EDIT__
