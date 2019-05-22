// AboutDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "USBTest.h"
#include "AboutDlg.h"
#include "afxdialogex.h"


// AboutDlg 对话框

IMPLEMENT_DYNAMIC(AboutDlg, CDialogEx)

AboutDlg::AboutDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_ABOUT_DIALOG, pParent)
{

}

AboutDlg::~AboutDlg()
{
}

void AboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_CPLD_VERSION, m_cpld_version);
	DDX_Control(pDX, IDC_STATIC_USB_VERSION, m_usb_version);
	
}

void AboutDlg::setCpldVersion(CString str) {
	str_cpld_version = str;
}
void AboutDlg::setUsbVersion(CString str) {
	str_usb_version = str;
}

BEGIN_MESSAGE_MAP(AboutDlg, CDialogEx)
	ON_STN_CLICKED(IDC_STATIC_WEB_SITE, &AboutDlg::OnStnClickedStaticWebSite)
	ON_STN_CLICKED(IDC_STATIC_GITHUB, &AboutDlg::OnStnClickedStaticGithub)
END_MESSAGE_MAP()


// AboutDlg 消息处理程序


BOOL AboutDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	if (str_cpld_version) 
		m_cpld_version.SetWindowTextA(str_cpld_version);
	if (str_usb_version) 
		m_usb_version.SetWindowTextA(str_usb_version);
	
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void AboutDlg::OnStnClickedStaticWebSite()
{
	// TODO: 在此添加控件通知处理程序代码
	ShellExecute(0, NULL, _T("www.arducam.com"), NULL, NULL, SW_NORMAL);
}


void AboutDlg::OnStnClickedStaticGithub()
{
	// TODO: 在此添加控件通知处理程序代码
	ShellExecute(0, NULL, _T("www.github.com/arducam"), NULL, NULL, SW_NORMAL);
}
