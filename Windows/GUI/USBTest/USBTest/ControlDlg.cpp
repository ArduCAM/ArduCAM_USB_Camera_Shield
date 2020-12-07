#include "stdafx.h"
#include "USBTest.h"
#include "ControlDlg.h"
#include "afxdialogex.h"
#include "USBTestDlg.h"
#include "ArduCamConfigLib.h"


IMPLEMENT_DYNAMIC(ControlDlg, CDialogEx)

ControlDlg::ControlDlg(ArduCamHandle usbCameraHd, ArduCamCfgHandle	usbCameraCfgHd, CWnd* pParent)
	: m_cUsbCameraHd(usbCameraHd), m_cUsbCameraCfgHd(usbCameraCfgHd), CDialogEx(IDD_CONTROL_DIALOG, pParent)
{
	SliderCtrlUI tmp[] = {
		{0, {IDC_CTRL_LABEL_1, new CStatic()}, {IDC_CTRL_SLIDER_1, new CSliderCtrl()}, {IDC_CTRL_EDIT_1, new CEdit()}},
		{0, {IDC_CTRL_LABEL_2, new CStatic()}, {IDC_CTRL_SLIDER_2, new CSliderCtrl()}, {IDC_CTRL_EDIT_2, new CEdit()}},
		{0, {IDC_CTRL_LABEL_3, new CStatic()}, {IDC_CTRL_SLIDER_3, new CSliderCtrl()}, {IDC_CTRL_EDIT_3, new CEdit()}},
		{0, {IDC_CTRL_LABEL_4, new CStatic()}, {IDC_CTRL_SLIDER_4, new CSliderCtrl()}, {IDC_CTRL_EDIT_4, new CEdit()}},
		{0, {IDC_CTRL_LABEL_5, new CStatic()}, {IDC_CTRL_SLIDER_5, new CSliderCtrl()}, {IDC_CTRL_EDIT_5, new CEdit()}},
		{0, {IDC_CTRL_LABEL_6, new CStatic()}, {IDC_CTRL_SLIDER_6, new CSliderCtrl()}, {IDC_CTRL_EDIT_6, new CEdit()}},
		{0, {IDC_CTRL_LABEL_7, new CStatic()}, {IDC_CTRL_SLIDER_7, new CSliderCtrl()}, {IDC_CTRL_EDIT_7, new CEdit()}},
	};
	for (int i = 0; i < sizeof(tmp) / sizeof(tmp[0]); i++)
		this->ctrls.push_back(tmp[i]);
}

ControlDlg::~ControlDlg()
{
}

void ControlDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	for (int i = 0; i < ctrls.size(); i++) {
		if (ctrls[i].type == 0) {
			DDX_Control(pDX, ctrls[i].label.id, *ctrls[i].label.widget);
			DDX_Control(pDX, ctrls[i].slider.id, *ctrls[i].slider.widget);
			DDX_Control(pDX, ctrls[i].edit.id, *ctrls[i].edit.widget);
		}
	}
}


BEGIN_MESSAGE_MAP(ControlDlg, CDialogEx)
	ON_BN_CLICKED(ID_CLOSE_CTRL, &ControlDlg::OnBnClickedCloseCtrl)
	ON_WM_HSCROLL()
END_MESSAGE_MAP()

void ControlDlg::OnBnClickedCloseCtrl()
{
	CDialogEx::OnCancel();
}


void ControlDlg::OnOK()
{
	//CDialogEx::OnOK();
}


BOOL ControlDlg::PreTranslateMessage(MSG* pMsg)
{
	CString csTmpString;
	Uint32 u32Value;
	if ((pMsg->message == WM_KEYDOWN) && (pMsg->wParam == VK_RETURN))
	{
		for (int i = 0; i < ctrls.size(); i++) {
			if (GetFocus() == GetDlgItem(ctrls[i].edit.id)) {
				ctrls[i].edit.widget->GetWindowText(csTmpString);
				u32Value = _ttoi(csTmpString);
				SetControl(ctrls[i].type, &u32Value);
				SetDlgItemInt(ctrls[i].edit.id, u32Value);
				ctrls[i].slider.widget->SetPos(u32Value);
			}
		}
		return TRUE;
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}


void ControlDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
	Uint32 value;

	for (int i = 0; i < ctrls.size(); i++) {
		if ((CSliderCtrl *)pScrollBar == ctrls[i].slider.widget) {
			value = ctrls[i].slider.widget->GetPos();
			SetDlgItemInt(ctrls[i].edit.id, value);
			TRACE("exposure: %d\n", value);
			SetControl(ctrls[i].type, &value);
		}
	}
}

Uint32 ControlDlg::SetControl(Uint32 u32Index, Uint32 *u32Value) {
	CtrlConfigPara *pu32ConfigData;
	if (ArduCamCfg_GetControl(m_cUsbCameraCfgHd, m_cUsbCameraHd, u32Index, &pu32ConfigData) != CFG_CONTROL_OK)
		return CFG_CONTROL_ERROR;

	if (*u32Value < pu32ConfigData->s32Min)
		*u32Value = pu32ConfigData->s32Min;
	else if (*u32Value > pu32ConfigData->s32Max)
		*u32Value = pu32ConfigData->s32Max;
	pu32ConfigData->s32Val = *u32Value;
	return ArduCamCfg_SetControl(m_cUsbCameraCfgHd, m_cUsbCameraHd, u32Index, *u32Value);
}


void ControlDlg::InitWeights(ArduCamHandle usbCameraHd, ArduCamCfgHandle usbCameraCfgHd) {
	this->m_cUsbCameraHd = usbCameraHd;
	this->m_cUsbCameraCfgHd = usbCameraCfgHd;

	CtrlConfigPara *pu32ConfigData;

	for (int i = 0; i < ctrls.size(); i++) {
		if (ArduCamCfg_GetControl(m_cUsbCameraCfgHd, m_cUsbCameraHd, i, &pu32ConfigData) == CFG_CONTROL_OK) {
			ctrls[i].type = i;
			ctrls[i].slider.widget->SetRange(pu32ConfigData->s32Min, pu32ConfigData->s32Max);
			ctrls[i].label.widget->SetWindowTextA(pu32ConfigData->s8CtrlName);
			ctrls[i].slider.widget->SetPos(pu32ConfigData->s32Val);
			SetDlgItemInt(ctrls[i].edit.id, pu32ConfigData->s32Val);
			ctrls[i].label.widget->EnableWindow(TRUE);
			ctrls[i].slider.widget->EnableWindow(TRUE);
			ctrls[i].edit.widget->EnableWindow(TRUE);
		}
		else {
			ctrls[i].type = 0;
			ctrls[i].slider.widget->SetRange(0, 1);
			ctrls[i].slider.widget->SetPos(0);
			ctrls[i].label.widget->SetWindowTextA("None");
			SetDlgItemInt(ctrls[i].edit.id, 0);

			ctrls[i].label.widget->EnableWindow(FALSE);
			ctrls[i].slider.widget->EnableWindow(FALSE);
			ctrls[i].edit.widget->EnableWindow(FALSE);
		}
	}
}

BOOL ControlDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	if (m_cUsbCameraHd != NULL && m_cUsbCameraCfgHd != NULL)
		InitWeights(m_cUsbCameraHd, m_cUsbCameraCfgHd);

	return TRUE;
}
