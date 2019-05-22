/*
 * TTComboBox.h
 * 
 * To use:
 *
 * The class CTTComboBox derive from MFC class - CCombobox,
 * there is only extended tool-tip functionality for the list-box 
 * in the combobox. you can immediately use the class CTTComboBox 
 * as well as make new class derive from it.
 *
 * Description:
 *
 * Install hooks for window subclassing, all messages go to new WindowProc
 * (window procedure) before going to the combobox, Usually, all windows 
 * with the same class share one window procedure.
 * There is using the new WindowProc to hold up the List-box's messages and 
 * to do too-tip things.
 *
 * Updates:
 * 27th Sep, 2006
 * There are two extra features for the class CTTCombobox, it now supports two features as below:
 * 1. Allows that you align the listbox by the three modes CTTCombobox::alignLeft, CTTCombobox::alignRight, CTTCombobox::alignMiddle, 
 * Using the function SetAlignStyle to do this thing.
 * 2. To disable or enable any items in combobox, you only simply perform the function SetDisabledItem. 
 *    Note: that's not effective when the combobox haven't the style CBS_OWNERDRAWFIXED, so I suggest you should set the style, 
 *    there are three methods to gain the style CBS_OWNERDRAWFIXED, 
 *    a. you directly check the style for the combobox in resource. 
 *    b. If the combobox was created dynamically that you should do the thing in OnCreate(LPCREATESTRUCT lpCreateStruct), 
 *       Usually, you write the code lpCreateStruct->style |= CBS_OWNERDRAWFIXED | CBS_HASSTRINGS; at the first line in the function OnCreate(LPCREATESTRUCT lpCreateStruct).
 *    c. If the combobox was subclassed dynamically then you add the code - ModifyStyle(0, CBS_OWNERDRAWFIXED | CBS_HASSTRINGS); at the first line in PreSubclassWindow().
*/
#ifndef __TT_COMBOBOX__
#define __TT_COMBOBOX__

#include <afxwin.h>
#include <afxtempl.h>

/////////////////////////////////////////////////////////////////////////////
// CTTComboBox window
class CTTComboBox : public CComboBox
{
// Construction
public:
	CTTComboBox();

// Attributes
public:
	// Returns the edit-box within the combo, it might be NULL
	CEdit* GetEditBox();

	// Returns the list-box within the combo, it might be NULL
	CListBox* GetListBox();

	// Default, Align the left-edge of list-box with the combo,
	// also we can set other alignment style here.
	void SetAlignStyle(int style);

	// Different alignment style for list-box to be aligned with the combo
	static const int	alignLeft;
	static const int	alignRight;
	static const int	alignMiddle;

private:
	// Indicates alignment style for list-box to be aligned with the combo
	int				m_alignStyle;

	// The original option selected when mouse cursor moves 
	// on in the list-box in the Combobox 
	static int		m_OriginalSel;

	// Pointer to the tool-tip window for the list-box
	static CWnd		m_tipWnd;

	// The font for tool-tip window
	static CFont	m_font;

	// Indicate delayed time for show the tool-tip window
	// Note: All combobox with the same class will use the same delayed time.
	static UINT		m_nDelayTime;

	// Indicate whether or not the mouse cursor entered in which
	// window will own the tool-tip.
	static BOOL		m_isEnter;
	
	// A list-box/edit-box window is corresponding to a redefined window proc.
	static CMap<HWND, HWND, WNDPROC, WNDPROC&> m_mapWndProc;

	// A list-box is corresponding to this combobox.
	static CMap<HWND, HWND, CTTComboBox*, CTTComboBox*> m_mapCombo;

	// The coordinates of the button that contains drop-down arrow
	CRect	m_rcButton;

	// Indicate whether or not the too-tip window can be shown
	BOOL	m_isEnableTool;

	// The combobox information
	COMBOBOXINFO m_cbi;

	// The array to store disabled items
	CUIntArray	m_arrDisabledItems;

	CMap<int, int, CString, LPCTSTR> m_mapTipTexts;

public:
	// Set up the delayed time for show the tool-tip window
	static void SetTooltipDelay(UINT nDelay);

	// Get the delayed time for show the tool-tip window
	static UINT GetTooltipDelay();

// Operations
public:
	// Set the combobox own the tool-tip window
	void SetToolTip(BOOL bEnable=TRUE);

	// If you're no longer apply tool tip for specific item,
	// you need to assign NULL tip text again.
	void SetTipText(int nIndex, LPCTSTR pszTipText);

	BOOL SetDisabledItem(int nIndex, BOOL bDisabled = TRUE);

protected:
	// Create the tool-tip window
	void CreateTooltipWnd();

	// Destroy the tool-tip window
	// Note: it doesn't destroy the window object
	void DestroyTooltipWnd();

	// Install a hook to process messages send to the list-box and edit-box
	void InstallHookForListboxAndEditbox();

	// The callback function that hooks up and processes messages
	// send to the list-box within the combobox
	static LRESULT CALLBACK HookListboxWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	// The callback function that hooks up and processes messages
	// send to the edit-box within the combobox
	static LRESULT CALLBACK HookEditboxWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	// The callback function that hooks up and processes messages
	// send to the tool-tip window.
	static LRESULT CALLBACK HookTipWndProc(HWND hWnd, UINT message, WPARAM wp, LPARAM lp);

	// Handler for the mouse cursor moves on the list-box
	// that makes tool-tip for portion invisible option.
	static void HandleListboxMouseMove(CListBox* pList, UINT uFlag, CPoint point);	

	// Handler for the mouse cursor over the edit-box
	// that makes tool-tip for portion invisible text.
	// PARAMETER:
	// pWnd The window as edit box
	static void HandleEditboxMouseOver(CWnd* pWnd);

	// Handler for repaint the tool-tip window
	static void HandleOnPaint();

	// Handler for each interval specified when we used SetTimer to install a timer
	// for the tool-tip window.
	static void HandleOnTimer(UINT nIDEvent);

private:
	// Tracking mouse event on specified window
	// PARAMETER:
	// hWnd Handle to specified window
	// dwFlags Which events will be tracked
	static BOOL OnTrackMouseEvent(HWND hWnd, DWORD dwFlags);

	// By specified mode with alignment 
	void AlignListBoxWithCombo();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTTComboBox)
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTTComboBox();

	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);

	// Generated message map functions
protected:
	//{{AFX_MSG(CTTComboBox)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnDropdown();
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	//}}AFX_MSG
	afx_msg LRESULT OnMouseOver(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnMouseLeave(WPARAM wp, LPARAM lp);
	afx_msg void OnDestroy();

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

#endif // __TT_COMBOBOX__
