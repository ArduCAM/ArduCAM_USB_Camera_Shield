#pragma once

#include "ArduCamlib.h"
#include "ArduCamConfigLib.h"
#include <vector>
#include <array>

template <typename T>
struct WidgetMap{
	Uint32 id;
	T *widget;
};

struct SliderCtrlUI {
	Uint32 type;
	struct WidgetMap<CStatic> label;
	struct WidgetMap<CSliderCtrl> slider;
	struct WidgetMap<CEdit> edit;
};

class ControlDlg : public CDialogEx
{
	DECLARE_DYNAMIC(ControlDlg)

public:
	ControlDlg(ArduCamHandle usbCameraHd, ArduCamCfgHandle	usbCameraCfgHd, CWnd* pParent = nullptr);
	virtual ~ControlDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CONTROL_DIALOG };
#endif
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCloseCtrl();
	virtual void OnOK();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	void ControlDlg::InitWeights(ArduCamHandle usbCameraHd, ArduCamCfgHandle usbCameraCfgHd);
private:
	ArduCamHandle		m_cUsbCameraHd;
	ArduCamCfgHandle	m_cUsbCameraCfgHd;
	std::vector<SliderCtrlUI> ctrls;

	Uint32 ControlDlg::SetControl(Uint32 u32Index, Uint32 *u32Value);
public:
	virtual BOOL OnInitDialog();
};
