// DataInfoDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "USBTest.h"
#include "DataInfoDlg.h"
#include "afxdialogex.h"

#define USB_CPLD_I2C_ADDRESS 0x46

// DataInfoDlg 对话框

IMPLEMENT_DYNAMIC(DataInfoDlg, CDialogEx)

DataInfoDlg::DataInfoDlg(ArduCamHandle usbCameraHd, CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DATA_INFO_DIALOG, pParent), usbCameraHd(usbCameraHd)
{

}

DataInfoDlg::~DataInfoDlg()
{
}

void DataInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MIPI_DATA_ID_EDIT, m_MIPIDataID);
	DDX_Control(pDX, IDC_MIPI_DATA_ROW_EDIT, m_MIPIDataRow);
	DDX_Control(pDX, IDC_MIPI_DATA_COL_EDIT, m_MIPIDataCol);
	DDX_Control(pDX, IDC_MIPI_CLOCK_EDIT, m_MIPIClock);
}


BEGIN_MESSAGE_MAP(DataInfoDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &DataInfoDlg::OnBnClickedOk)
END_MESSAGE_MAP()


void DataInfoDlg::OnOK()
{
	//CDialogEx::OnOK();
}

void DataInfoDlg::SetUsbCameraHandle(ArduCamHandle usbCameraHd)
{
	this->usbCameraHd = usbCameraHd;
}

static BOOL haveCpld(ArduCamHandle useHandle) {
	Uint32 defData = 0, data = 0;
	ArduCam_readReg_8_8(useHandle, USB_CPLD_I2C_ADDRESS, 0x04, &defData);
	srand(time(NULL));
	int rand_data = 0;
	while ((rand_data = (rand() % 255)) == defData);

	ArduCam_writeReg_8_8(useHandle, USB_CPLD_I2C_ADDRESS, 0x04, rand_data);
	ArduCam_readReg_8_8(useHandle, USB_CPLD_I2C_ADDRESS, 0x04, &data);
	if (data == rand_data) {
		ArduCam_writeReg_8_8(useHandle, USB_CPLD_I2C_ADDRESS, 0x04, defData);
		return TRUE;
	}
	return FALSE;
}

void DataInfoDlg::ReadDataInfo()
{
	if (this->usbCameraHd == nullptr)
		return;

	if (!haveCpld(this->usbCameraHd))
		return;

	Uint32 version;

	ArduCam_readReg_8_8(this->usbCameraHd, USB_CPLD_I2C_ADDRESS, 00, &version);
	if (version < 0x25)
		return;

	Uint32 year, mouth, day;
	ArduCam_readReg_8_8(this->usbCameraHd, USB_CPLD_I2C_ADDRESS, 5, &year);
	ArduCam_readReg_8_8(this->usbCameraHd, USB_CPLD_I2C_ADDRESS, 6, &mouth);
	ArduCam_readReg_8_8(this->usbCameraHd, USB_CPLD_I2C_ADDRESS, 7, &day);
	
	Uint32 date = (year * 1000 + mouth * 100 + day);

	if ((version & 0xF0) != 0x20 && (version & 0xF0) != 0x30)
		return;

	if ((version & 0xF0) == 0x20 && date < (19 * 1000 + 7 * 100 + 8)) {
		return;
	}
	else if ((version & 0xF0) == 0x30 && date < (19 * 1000 + 3 * 100 + 22)) {
		return;
	}


	Uint32 dataID;
	CString csTmpString;

	ArduCam_readReg_8_8(usbCameraHd, USB_CPLD_I2C_ADDRESS, 0x1E, &dataID);
	csTmpString.Format("0x%02X", dataID);
	m_MIPIDataID.SetWindowTextA(csTmpString);

	Uint32 rowMSB, rowLSB, row;
	ArduCam_readReg_8_8(usbCameraHd, USB_CPLD_I2C_ADDRESS, 0x21, &rowMSB);
	ArduCam_readReg_8_8(usbCameraHd, USB_CPLD_I2C_ADDRESS, 0x22, &rowLSB);
	row = ((rowMSB & 0xFF) << 8) | (rowLSB & 0xFF);
	csTmpString.Format("%d", row);
	m_MIPIDataRow.SetWindowTextA(csTmpString);
		
	Uint32 colMSB, colLSB, col;
	ArduCam_readReg_8_8(usbCameraHd, USB_CPLD_I2C_ADDRESS, 0x1F, &colMSB);
	ArduCam_readReg_8_8(usbCameraHd, USB_CPLD_I2C_ADDRESS, 0x20, &colLSB);;
	col = ((colMSB & 0x1F) << 8) | (colLSB & 0xFF);
	csTmpString.Format("%d", col);
	m_MIPIDataCol.SetWindowTextA(csTmpString);

	// after 2020/06/22
	if ((version & 0xF0) == 0x20 && date < (20 * 1000 + 6 * 100 + 22)) return;
	else if ((version & 0xF0) == 0x30 && date < (20 * 1000 + 6 * 100 + 22)) return;

	Uint32 mipiClk;
	ArduCam_readReg_8_8(usbCameraHd, USB_CPLD_I2C_ADDRESS, 0x27, &mipiClk);
	csTmpString.Format("%d", mipiClk);
	m_MIPIClock.SetWindowTextA(csTmpString);
}

void DataInfoDlg::OnBnClickedOk()
{
	ReadDataInfo();
}


BOOL DataInfoDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	ReadDataInfo();
	return TRUE;
}
