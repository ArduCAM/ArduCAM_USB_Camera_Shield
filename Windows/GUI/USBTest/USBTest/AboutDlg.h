#pragma once


// AboutDlg 对话框
#include <afxwin.h>
#include "TTStatic.h"

class AboutDlg : public CDialogEx
{
	DECLARE_DYNAMIC(AboutDlg)

public:
	AboutDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~AboutDlg();
	void setCpldVersion(CString str);
	void setUsbVersion(CString str);
	void showPrompt(bool show);
	
// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUT_DIALOG };
#endif
private:
	CString str_cpld_version;
	CString str_usb_version;
	CStatic m_cpld_version;
	CStatic m_usb_version;
	CColorStatic m_static;
	int show_prompt;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnStnClickedStaticWebSite();
	afx_msg void OnStnClickedStaticGithub();
};
