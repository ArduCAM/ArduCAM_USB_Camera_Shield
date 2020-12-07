#pragma once
#include "ArduCamlib.h"

// DataInfoDlg 对话框

class DataInfoDlg : public CDialogEx
{
	DECLARE_DYNAMIC(DataInfoDlg)

public:
	DataInfoDlg(ArduCamHandle usbCameraHd = nullptr, CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~DataInfoDlg();

	ArduCamHandle usbCameraHd;
// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DATA_INFO_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
	virtual void OnOK();
public:
	afx_msg void OnBnClickedOk();
	CEdit m_MIPIDataID;
	CEdit m_MIPIDataRow;
	CEdit m_MIPIDataCol;
	void ReadDataInfo();
	void SetUsbCameraHandle(ArduCamHandle usbCameraHd);
	virtual BOOL OnInitDialog();
	CEdit m_MIPIClock;
};
