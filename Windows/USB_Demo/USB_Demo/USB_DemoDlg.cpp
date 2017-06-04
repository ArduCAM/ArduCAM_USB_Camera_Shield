// USBTestDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "USB_Demo.h"
#include "USB_DemoDlg.h"
//#include <chrono>
#include "sensor.h"
#include "MT9F002.h"
#include "MT9N001.h"
#include "MT9J001.h"
#include "OV7670.h"
#include "OV7725.h"
#include "AR0134.h"
#include "MT9V034.h"   


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CUSBTestDlg 对话框

UINT _FrameCaptureThread( LPVOID lParam );							// 图像采集线程
//UINT _IMUCaptureThread( LPVOID lParam );							// IMU采集线程
UINT _FrameReadThread( LPVOID lParam );								// 图像读取显示线程


CUSBTestDlg::CUSBTestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CUSBTestDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_cUsbCameraHd = NULL;

	m_pu8OutRgb24 = ( Uint8* )malloc( 4096 * 4096 * 3 );
}
 

void CUSBTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON_INIT, m_btInit);
	DDX_Control(pDX, IDC_RICHEDIT_LOG, m_reInf);
	DDX_Control(pDX, IDC_BUTTON_PLAY, m_btPlay);
	DDX_Control(pDX, IDC_BUTTON_STOP, m_btStop);
	DDX_Control(pDX, IDC_STATIC_FRAME_CAPTRUE_COUNT, m_sttFrameCaptureCnt);
	DDX_Control(pDX, IDC_STATIC_FRAME_READ_COUNT, m_sttFrameReadCnt);
	DDX_Control(pDX, IDC_STATIC_DISPLAY, m_sttDisplay);
	//DDX_Control(pDX, IDC_BUTTON_CLOSE, m_btClose);
	DDX_Control(pDX, IDC_EDIT4, m_edtRegAddr);
	DDX_Control(pDX, IDC_EDIT5, m_edtRegValue);
	DDX_Control(pDX, IDC_BUTTON_REG_READ, m_btRegRead);
	DDX_Control(pDX, IDC_BUTTON_REG_WRITE, m_btRegWrite);
	DDX_Control(pDX, IDC_COMBO2, m_cmbType);
	DDX_Control(pDX, IDC_EDIT1, m_edtWidth);
	DDX_Control(pDX, IDC_EDIT2, m_edtHeight);
	DDX_Control(pDX, IDC_COMBO1, m_cmbBits);
	DDX_Control(pDX, IDC_BUTTON_SCAN, m_btScan);
	DDX_Control(pDX, IDC_COMBO3, m_cmbUsbIndex);
	DDX_Control(pDX, IDC_COMBO4, m_cmbVID);
	DDX_Control(pDX, IDC_BUTTON_OPEN, m_btOpen);
	DDX_Control(pDX, IDC_BUTTON_CLOSE, m_btClose);
	DDX_Control(pDX, IDC_BUTTON_BYTE_CONV, m_btRawMode);
	DDX_Control(pDX, IDC_COMBO5, m_cmbI2cMode);
	DDX_Control(pDX, IDC_EDIT3, m_edtShipAddr);
	//DDX_Control(pDX, IDC_BUTTON_SHOT, m_btShot);
	//DDX_Control(pDX, IDC_STATIC_SHOT_TIME, m_sttShotTime);
	DDX_Control(pDX, IDC_CHECK1, m_chkForceDisp);
	/*DDX_Control(pDX, IDC_STATIC_IMU_INDEX, m_sttIMUIdx);
	DDX_Control(pDX, IDC_STATIC_IMU_TIME, m_sttIMUTime);
	DDX_Control(pDX, IDC_STATIC_IMU_ACCX, m_sttIMUAccX);
	DDX_Control(pDX, IDC_STATIC_IMU_ACCY, m_sttIMUAccY);
	DDX_Control(pDX, IDC_STATIC_IMU_ACCZ, m_sttIMUAccZ);
	DDX_Control(pDX, IDC_STATIC_IMU_GYROX, m_sttIMUGyroX);
	DDX_Control(pDX, IDC_STATIC_IMU_GYROY, m_sttIMUGyroY);
	DDX_Control(pDX, IDC_STATIC_IMU_GYROZ, m_sttIMUGyroZ);
	DDX_Control(pDX, IDC_STATIC_IMU_TIMP, m_sttIMUTemp);*/
	DDX_Control(pDX, IDC_COMBO6, m_cmbSensor);
}
 

BEGIN_MESSAGE_MAP(CUSBTestDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_INIT, &CUSBTestDlg::OnBnClickedButtonInit)
	ON_BN_CLICKED(IDC_BUTTON_PLAY, &CUSBTestDlg::OnBnClickedButtonPlay)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_STOP, &CUSBTestDlg::OnBnClickedButtonStop)
	ON_BN_CLICKED(IDC_BUTTON_CLOSE, &CUSBTestDlg::OnBnClickedButtonClose)
	ON_BN_CLICKED(IDC_BUTTON_REG_READ, &CUSBTestDlg::OnBnClickedButtonRegRead)
	ON_BN_CLICKED(IDC_BUTTON_REG_WRITE, &CUSBTestDlg::OnBnClickedButtonRegWrite)
	ON_BN_CLICKED(IDC_BUTTON_SCAN, &CUSBTestDlg::OnBnClickedButtonScan)
	ON_BN_CLICKED(IDC_BUTTON_OPEN, &CUSBTestDlg::OnBnClickedButtonOpen)
	//ON_BN_CLICKED(IDOK, &CUSBTestDlg::OnBnClickedOk)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON_BYTE_CONV, &CUSBTestDlg::OnBnClickedButtonByteConv)
	ON_BN_CLICKED(IDC_BUTTON_SHOT, &CUSBTestDlg::OnBnClickedButtonShot)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEWHEEL()
	ON_BN_CLICKED(IDC_CHECK1, &CUSBTestDlg::OnBnClickedCheck1)
	ON_CBN_SELCHANGE(IDC_COMBO6, &CUSBTestDlg::OnCbnSelchangeCombo6)
END_MESSAGE_MAP()


// CUSBTestDlg 消息处理程序

BOOL CUSBTestDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	m_u32FrameCaptureThreadEn = THREAD_DISABLE;
	m_u32FrameReadThreadEn    = THREAD_DISABLE;
	
	m_u32IMUCaptureThreadEn = THREAD_DISABLE;

	m_u32FrameCaptureCnt = 0;
	m_u32FrameReadCnt    = 0;

	m_u32FrameCaptureCntBak = 0;
	m_u32FrameReadCntBak    = 0;

	m_btPlay.EnableWindow( FALSE );
	m_btStop.EnableWindow( FALSE );
	m_btClose.EnableWindow( FALSE );
	m_btShot.EnableWindow( FALSE );
	m_btRegRead.EnableWindow( FALSE );
	m_btRegWrite.EnableWindow( FALSE );

	m_chkForceDisp.EnableWindow( FALSE );

	m_cmbVID.SetCurSel( 0 );
	m_cmbType.SetCurSel( 0 );
	m_cmbBits.SetCurSel( 0 );

	

	m_cmbSensor.AddString("OV7670");
	m_cmbSensor.AddString("OV7725");
	m_cmbSensor.AddString("MT9V034");
	m_cmbSensor.AddString("AR0134");
	
	m_cmbSensor.AddString("MT9J001");
	m_cmbSensor.AddString("MT9F002");
	m_cmbSensor.AddString("MT9N001");
	

	m_cmbSensor.SetCurSel(MT9V034);
	OnCbnSelchangeCombo6();

	SetTimer( DISP_INF_TIMER, 1000, NULL );

	m_u32ShotFlag  = 0;
	m_u32CaptFlag  = 0;

	m_u32FrameLeftloc  = 250;
	m_u32FrameRightloc = 250 + 640; 
	m_u32FrameUploc	   = 10;
	m_u32FrameDownloc  = 10 + 480;

	m_s32LocXMoved = 0xFFFF;
	m_s32LocYMoved = 0xFFFF;

	m_s32locStartX = 0;
	m_s32locStartY = 0;

	m_pixScale = 0.5;
	m_s32ScaleIndex = 10;

	m_sttDisplay.SetWindowPos( NULL, m_u32FrameLeftloc, m_u32FrameUploc, 640, 480, 0 );

	/* ----- 绘制黑色背景 ----- */
	CDC		*pcdDC;
	CRect	crRect;	
	CWnd	*pcwWnd = GetDlgItem( IDC_STATIC_DISPLAY );
	pcwWnd->GetClientRect( &crRect );
	pcdDC = pcwWnd->GetDC();

	BITMAPINFOHEADER bmiHeader;
	bmiHeader.biSize		  = sizeof(BITMAPINFOHEADER);
	bmiHeader.biWidth		  = 640;
	bmiHeader.biHeight		  = 480;
	bmiHeader.biPlanes		  = 1;
	bmiHeader.biBitCount	  = 24;
	bmiHeader.biCompression	  = BI_RGB;
	bmiHeader.biSizeImage	  = 0;
	bmiHeader.biXPelsPerMeter = 0;
	bmiHeader.biYPelsPerMeter = 0;
	bmiHeader.biClrUsed		  = 0;
	bmiHeader.biClrImportant  = 0;		
	SetStretchBltMode( pcdDC->m_hDC, HALFTONE );
	StretchDIBits( pcdDC->m_hDC, 
		           crRect.left, crRect.top, crRect.left+640, crRect.top+480, 
		           0, 0, 640, 480, 
		           NULL, ( BITMAPINFO* )&bmiHeader, BI_RGB, BLACKNESS );	
	/* ----- 绘制黑色背景 ----- */

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}
 

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CUSBTestDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}
  

//当用户拖动最小化窗口时系统调用此函数取得光标显示。
//
HCURSOR CUSBTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}
  

void CUSBTestDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	switch (nIDEvent)
	{
		/* --------------------------------------------------------------------------------------------------- */
		/* ----- 自己编写的部分																			 ----- */
		/* --------------------------------------------------------------------------------------------------- */
		case	DISP_INF_TIMER:				DispInfo();					break;				
		default:														break;
		/* --------------------------------------------------------------------------------------------------- */
	}

	CDialog::OnTimer(nIDEvent);
}
 

void CUSBTestDlg::InsertText( CString str, COLORREF rgb )
{
	m_reInf.SetSel(0,0);
	
	CHARFORMAT cf;
	m_reInf.GetDefaultCharFormat(cf);
	strcpy(cf.szFaceName,_T("宋体"));
	cf.dwEffects   = 0;
	cf.yHeight	   = 180;
	cf.cbSize      = 18;
	cf.crTextColor = rgb;
	m_reInf.SetWordCharFormat(cf);

	SYSTEMTIME time;
	GetLocalTime( &time );
	CString strTime;
	strTime.Format( "[%02d:%02d:%02d]", time.wHour, time.wMinute, time.wSecond );
	str = strTime + str + "\n";

	m_reInf.SetSel(0,0);
	m_reInf.SetWordCharFormat(cf);
	m_reInf.ReplaceSel( str );

	if( m_reInf.GetTextLength() > 2000 )
	{
		/* 删除一半长度 */
		m_reInf.SetSel(m_reInf.GetTextLength() / 2 ,-1);
		m_reInf.ReplaceSel( _T("" ) );
	}

	m_reInf.SetSel(0,0);
}
  

void CUSBTestDlg::DispErrorLog( Uint32 u32ErrorCode )
{
	CString csErrorCode;

	switch ( u32ErrorCode )
	{
		case USB_CAMERA_NO_ERROR :																			break;

		case USB_CAMERA_USB_CREATE_ERROR :		csErrorCode.Format( "USB device create error" );			break;
		case USB_CAMERA_USB_SET_CONTEXT_ERROR :	csErrorCode.Format( "USB camera context set error" );		break;
		case USB_CAMERA_VR_COMMAND_ERROR :		csErrorCode.Format( "USB camera VR command error" );		break;
		case USB_CAMERA_USB_VERSION_ERROR :		csErrorCode.Format( "USB version error" );					break;
		case USB_CAMERA_BUFFER_ERROR :			csErrorCode.Format( "USB camera buffer create error" );		break;

		case USB_CAMERA_I2C_BIT_ERROR :			csErrorCode.Format( "USB camera I2C bit error" );			break;
		case USB_CAMERA_I2C_NACK_ERROR :		csErrorCode.Format( "USB camera I2C nack error" );			break;
		case USB_CAMERA_I2C_TIMEOUT :			csErrorCode.Format( "USB camera I2C timeout error" );		break;

		case USB_CAMERA_USB_TASK_ERROR :		csErrorCode.Format( "USB transfer task create error" );		break;
		case USB_CAMERA_DATA_OVERFLOW_ERROR :	csErrorCode.Format( "USB transfer data overflow error" );	break;
		case USB_CAMERA_DATA_LACK_ERROR :		csErrorCode.Format( "USB transfer data lack error" );		break;
		case USB_CAMERA_FIFO_FULL_ERROR :		csErrorCode.Format( "USB transfer FIFO full error" );		break;
		case USB_CAMERA_DATA_LEN_ERROR:			csErrorCode.Format( "USB transfer data length error" );		break;
		case USB_CAMERA_FRAME_INDEX_ERROR:		csErrorCode.Format( "USB transfer data index error" );		break;

		case USB_CAMERA_READ_EMPTY_ERROR :		csErrorCode.Format( "camera frame read empty error" );		break;
		case USB_CAMERA_DEL_EMPTY_ERROR :		csErrorCode.Format( "camera frame delete empty error" );	break;

		case USB_CAMERA_SIZE_EXCEED_ERROR:		csErrorCode.Format( "camera size exceed error" );			break;

		default :								csErrorCode.Format( "unknown error" );						break;
	}

	if ( u32ErrorCode != USB_CAMERA_NO_ERROR )		InsertText( csErrorCode, OUTPUT_RED );

}
  

void CUSBTestDlg::DispInfo( void )
{
	CString csTmpString;

	csTmpString.Format( "frame capture count: %d fps", m_u32FrameCaptureCnt - m_u32FrameCaptureCntBak );
	m_sttFrameCaptureCnt.SetWindowText( csTmpString );

	csTmpString.Format( "frame read count: %d fps", m_u32FrameReadCnt - m_u32FrameReadCntBak );
	m_sttFrameReadCnt.SetWindowText( csTmpString );

	m_u32FrameCaptureCntBak = m_u32FrameCaptureCnt;
	m_u32FrameReadCntBak    = m_u32FrameReadCnt;
}
  

void CUSBTestDlg::Raw2rgb24( Uint8 *pu8DstBmpData, Uint8 *pu8ResRawData, Uint32 u32Width, Uint32 u32Height, Uint8 u8RawMode, Uint8 u8PixelBits )
{
	Uint32 u32TmpHeight = u32Height;
	Uint32 u32TmpWidth  = u32Width;

	Uint32 u32UseStartG1H;
	Uint32 u32UseStartG1W;
	Uint32 u32UseStartRH;
	Uint32 u32UseStartRW;
	Uint32 u32UseStartBH;
	Uint32 u32UseStartBW;
	Uint32 u32UseStartG2H;
	Uint32 u32UseStartG2W;

	switch ( u8RawMode )
	{
		case RAW_RG:	u32UseStartG1H = 2;
						u32UseStartG1W = 1;
						u32UseStartRH  = 2;
						u32UseStartRW  = 2;
						u32UseStartBH  = 1;
						u32UseStartBW  = 1;
						u32UseStartG2H = 1;
						u32UseStartG2W = 2;
						break;

		case RAW_GR:	u32UseStartG1H = 2;
						u32UseStartG1W = 2;
						u32UseStartRH  = 2;
						u32UseStartRW  = 1;
						u32UseStartBH  = 1;
						u32UseStartBW  = 2;
						u32UseStartG2H = 1;
						u32UseStartG2W = 1;
						break;

		case RAW_GB:	u32UseStartG1H = 1;
						u32UseStartG1W = 1;
						u32UseStartRH  = 1;
						u32UseStartRW  = 2;
						u32UseStartBH  = 2;
						u32UseStartBW  = 1;
						u32UseStartG2H = 2;
						u32UseStartG2W = 2;
						break;

		case RAW_BG:	u32UseStartG1H = 1;
						u32UseStartG1W = 2;
						u32UseStartRH  = 1;
						u32UseStartRW  = 1;
						u32UseStartBH  = 2;
						u32UseStartBW  = 2;
						u32UseStartG2H = 2;
						u32UseStartG2W = 1;
						break;

		default:    return;
	}
	
	/* -- raw转换bmp -- */
	if ( u8PixelBits <= 8 )
	{
		/* -- G1 -- */
		for (Uint32 u32IndexI = u32UseStartG1H; u32IndexI < u32TmpHeight - 1; u32IndexI += 2)
		{
			for (Uint32 u32IndexJ = u32UseStartG1W; u32IndexJ < u32TmpWidth - 1; u32IndexJ += 2)
			{
				pu8DstBmpData[(u32TmpHeight - u32IndexI)*u32TmpWidth * 3 + (u32TmpWidth - u32IndexJ) * 3 + 0] = (   pu8ResRawData[ u32IndexI   *u32TmpWidth + u32IndexJ+1]
																										  	      + pu8ResRawData[ u32IndexI   *u32TmpWidth + u32IndexJ-1] ) >> 1 << ( u8PixelBits - 8 );
				pu8DstBmpData[(u32TmpHeight - u32IndexI)*u32TmpWidth * 3 + (u32TmpWidth - u32IndexJ) * 3 + 1] =     pu8ResRawData[ u32IndexI   *u32TmpWidth + u32IndexJ  ]		  << ( u8PixelBits - 8 );
				pu8DstBmpData[(u32TmpHeight - u32IndexI)*u32TmpWidth * 3 + (u32TmpWidth - u32IndexJ) * 3 + 2] = (   pu8ResRawData[(u32IndexI+1)*u32TmpWidth + u32IndexJ  ]
																											      + pu8ResRawData[(u32IndexI-1)*u32TmpWidth + u32IndexJ  ] ) >> 1 << ( u8PixelBits - 8 ); 
			}
		}

		/* -- R -- */
		for (Uint32 u32IndexI = u32UseStartRH; u32IndexI < u32TmpHeight - 1; u32IndexI += 2)
		{
			for (Uint32 u32IndexJ = u32UseStartRW; u32IndexJ < u32TmpWidth - 1; u32IndexJ += 2)
			{
				pu8DstBmpData[(u32TmpHeight - u32IndexI)*u32TmpWidth * 3 + (u32TmpWidth - u32IndexJ) * 3 + 0] =     pu8ResRawData[ u32IndexI   *u32TmpWidth + u32IndexJ  ]		  << ( u8PixelBits - 8 );
				pu8DstBmpData[(u32TmpHeight - u32IndexI)*u32TmpWidth * 3 + (u32TmpWidth - u32IndexJ) * 3 + 1] = (   pu8ResRawData[ u32IndexI   *u32TmpWidth + u32IndexJ+1]
																												  + pu8ResRawData[ u32IndexI   *u32TmpWidth + u32IndexJ-1]
																												  + pu8ResRawData[(u32IndexI+1)*u32TmpWidth + u32IndexJ  ]
																												  + pu8ResRawData[(u32IndexI-1)*u32TmpWidth + u32IndexJ  ] ) >> 2 << ( u8PixelBits - 8 );
				pu8DstBmpData[(u32TmpHeight - u32IndexI)*u32TmpWidth * 3 + (u32TmpWidth - u32IndexJ) * 3 + 2] = (   pu8ResRawData[(u32IndexI+1)*u32TmpWidth + u32IndexJ+1]
																												  + pu8ResRawData[(u32IndexI+1)*u32TmpWidth + u32IndexJ-1]
																												  + pu8ResRawData[(u32IndexI-1)*u32TmpWidth + u32IndexJ+1]
																												  + pu8ResRawData[(u32IndexI-1)*u32TmpWidth + u32IndexJ-1] ) >> 2 << ( u8PixelBits - 8 );
			}
		}

		/* -- B -- */
		for (Uint32 u32IndexI = u32UseStartBH; u32IndexI < u32TmpHeight - 1; u32IndexI += 2)
		{
			for (Uint32 u32IndexJ = u32UseStartBW; u32IndexJ < u32TmpWidth - 1; u32IndexJ += 2)
			{
				pu8DstBmpData[(u32TmpHeight - u32IndexI)*u32TmpWidth * 3 + (u32TmpWidth - u32IndexJ) * 3 + 0] = (   pu8ResRawData[(u32IndexI+1)*u32TmpWidth + u32IndexJ+1]
																												  + pu8ResRawData[(u32IndexI+1)*u32TmpWidth + u32IndexJ-1]
																												  + pu8ResRawData[(u32IndexI-1)*u32TmpWidth + u32IndexJ+1]
																												  + pu8ResRawData[(u32IndexI-1)*u32TmpWidth + u32IndexJ-1] ) >> 2 << ( u8PixelBits - 8 ); 
				pu8DstBmpData[(u32TmpHeight - u32IndexI)*u32TmpWidth * 3 + (u32TmpWidth - u32IndexJ) * 3 + 1] = (   pu8ResRawData[ u32IndexI   *u32TmpWidth + u32IndexJ+1]
																												  + pu8ResRawData[ u32IndexI   *u32TmpWidth + u32IndexJ-1]
																												  + pu8ResRawData[(u32IndexI+1)*u32TmpWidth + u32IndexJ  ]
																												  + pu8ResRawData[(u32IndexI-1)*u32TmpWidth + u32IndexJ  ] ) >> 2 << ( u8PixelBits - 8 ); 
				pu8DstBmpData[(u32TmpHeight - u32IndexI)*u32TmpWidth * 3 + (u32TmpWidth - u32IndexJ) * 3 + 2] =     pu8ResRawData[ u32IndexI   *u32TmpWidth + u32IndexJ  ]        << ( u8PixelBits - 8 );
			}
		}

		/* -- G2 -- */
		for (Uint32 u32IndexI = u32UseStartG2H; u32IndexI < u32TmpHeight - 1; u32IndexI += 2)
		{
			for (Uint32 u32IndexJ = u32UseStartG2W; u32IndexJ < u32TmpWidth - 1; u32IndexJ += 2)
			{
				pu8DstBmpData[(u32TmpHeight - u32IndexI)*u32TmpWidth * 3 + (u32TmpWidth - u32IndexJ) * 3 + 0] = (   pu8ResRawData[(u32IndexI+1)*u32TmpWidth + u32IndexJ  ]
																												  + pu8ResRawData[(u32IndexI-1)*u32TmpWidth + u32IndexJ  ] ) >> 1 << ( u8PixelBits - 8 );
				pu8DstBmpData[(u32TmpHeight - u32IndexI)*u32TmpWidth * 3 + (u32TmpWidth - u32IndexJ) * 3 + 1] =     pu8ResRawData[ u32IndexI   *u32TmpWidth + u32IndexJ  ]		  << ( u8PixelBits - 8 ); 
				pu8DstBmpData[(u32TmpHeight - u32IndexI)*u32TmpWidth * 3 + (u32TmpWidth - u32IndexJ) * 3 + 2] = (   pu8ResRawData[ u32IndexI   *u32TmpWidth + u32IndexJ+1]
																												  + pu8ResRawData[ u32IndexI   *u32TmpWidth + u32IndexJ-1] ) >> 1 << ( u8PixelBits - 8 );
			}
		}
	}
	else if ( u8PixelBits <= 16 )
	{
		Uint16* pu16ResRawData = ( Uint16* )pu8ResRawData;
		 
		/* -- G1 -- */
		for (Uint32 u32IndexI = u32UseStartG1H; u32IndexI < u32TmpHeight - 1; u32IndexI += 2)
		{
			for (Uint32 u32IndexJ = u32UseStartG1W; u32IndexJ < u32TmpWidth - 1; u32IndexJ += 2)
			{
				pu8DstBmpData[(u32TmpHeight - u32IndexI)*u32TmpWidth * 3 + (u32TmpWidth - u32IndexJ) * 3 + 0] = (   pu16ResRawData[ u32IndexI   *u32TmpWidth + u32IndexJ+1]
																												  + pu16ResRawData[ u32IndexI   *u32TmpWidth + u32IndexJ-1] ) >> 1 >> ( u8PixelBits - 8 );
				pu8DstBmpData[(u32TmpHeight - u32IndexI)*u32TmpWidth * 3 + (u32TmpWidth - u32IndexJ) * 3 + 1] =     pu16ResRawData[ u32IndexI   *u32TmpWidth + u32IndexJ  ]		   >> ( u8PixelBits - 8 );
				pu8DstBmpData[(u32TmpHeight - u32IndexI)*u32TmpWidth * 3 + (u32TmpWidth - u32IndexJ) * 3 + 2] = (   pu16ResRawData[(u32IndexI+1)*u32TmpWidth + u32IndexJ  ]
  																												  + pu16ResRawData[(u32IndexI-1)*u32TmpWidth + u32IndexJ  ] ) >> 1 >> ( u8PixelBits - 8 );
			}
		}

		/* -- R -- */
		for (Uint32 u32IndexI = u32UseStartRH; u32IndexI < u32TmpHeight - 1; u32IndexI += 2)
		{
			for (Uint32 u32IndexJ = u32UseStartRW; u32IndexJ < u32TmpWidth - 1; u32IndexJ += 2)
			{
				pu8DstBmpData[(u32TmpHeight - u32IndexI)*u32TmpWidth * 3 + (u32TmpWidth - u32IndexJ) * 3 + 0] =     pu16ResRawData[ u32IndexI   *u32TmpWidth + u32IndexJ  ]		   >> ( u8PixelBits - 8 );
				pu8DstBmpData[(u32TmpHeight - u32IndexI)*u32TmpWidth * 3 + (u32TmpWidth - u32IndexJ) * 3 + 1] = (   pu16ResRawData[ u32IndexI   *u32TmpWidth + u32IndexJ+1]
																												  + pu16ResRawData[ u32IndexI   *u32TmpWidth + u32IndexJ-1]
																												  + pu16ResRawData[(u32IndexI+1)*u32TmpWidth + u32IndexJ  ]
																												  + pu16ResRawData[(u32IndexI-1)*u32TmpWidth + u32IndexJ  ] ) >> 2 >> ( u8PixelBits - 8 );
				pu8DstBmpData[(u32TmpHeight - u32IndexI)*u32TmpWidth * 3 + (u32TmpWidth - u32IndexJ) * 3 + 2] = (   pu16ResRawData[(u32IndexI+1)*u32TmpWidth + u32IndexJ+1]
																												  + pu16ResRawData[(u32IndexI+1)*u32TmpWidth + u32IndexJ-1]
																												  + pu16ResRawData[(u32IndexI-1)*u32TmpWidth + u32IndexJ+1]
																												  + pu16ResRawData[(u32IndexI-1)*u32TmpWidth + u32IndexJ-1] ) >> 2 >> ( u8PixelBits - 8 ); 
			}
		}

		/* -- B -- */
		for (Uint32 u32IndexI = u32UseStartBH; u32IndexI < u32TmpHeight - 1; u32IndexI += 2)
		{
			for (Uint32 u32IndexJ = u32UseStartBW; u32IndexJ < u32TmpWidth - 1; u32IndexJ += 2)
			{
				pu8DstBmpData[(u32TmpHeight - u32IndexI)*u32TmpWidth * 3 + (u32TmpWidth - u32IndexJ) * 3 + 0] = (   pu16ResRawData[(u32IndexI+1)*u32TmpWidth + u32IndexJ+1]
																												  + pu16ResRawData[(u32IndexI+1)*u32TmpWidth + u32IndexJ-1]
																												  + pu16ResRawData[(u32IndexI-1)*u32TmpWidth + u32IndexJ+1]
																												  + pu16ResRawData[(u32IndexI-1)*u32TmpWidth + u32IndexJ-1] ) >> 2 >> ( u8PixelBits - 8 ); 
				pu8DstBmpData[(u32TmpHeight - u32IndexI)*u32TmpWidth * 3 + (u32TmpWidth - u32IndexJ) * 3 + 1] = (   pu16ResRawData[ u32IndexI   *u32TmpWidth + u32IndexJ+1]
																												  + pu16ResRawData[ u32IndexI   *u32TmpWidth + u32IndexJ-1]
																												  + pu16ResRawData[(u32IndexI+1)*u32TmpWidth + u32IndexJ  ]
																												  + pu16ResRawData[(u32IndexI-1)*u32TmpWidth + u32IndexJ  ] ) >> 2 >> ( u8PixelBits - 8 ); 
				pu8DstBmpData[(u32TmpHeight - u32IndexI)*u32TmpWidth * 3 + (u32TmpWidth - u32IndexJ) * 3 + 2] =     pu16ResRawData[ u32IndexI   *u32TmpWidth + u32IndexJ  ]		   >> ( u8PixelBits - 8 );
			}
		}

		/* -- G2 -- */
		for (Uint32 u32IndexI = u32UseStartG2H; u32IndexI < u32TmpHeight - 1; u32IndexI += 2)
		{
			for (Uint32 u32IndexJ = u32UseStartG2W; u32IndexJ < u32TmpWidth - 1; u32IndexJ += 2)
			{
				pu8DstBmpData[(u32TmpHeight - u32IndexI)*u32TmpWidth * 3 + (u32TmpWidth - u32IndexJ) * 3 + 0] = (   pu16ResRawData[(u32IndexI+1)*u32TmpWidth + u32IndexJ  ]
																												  + pu16ResRawData[(u32IndexI-1)*u32TmpWidth + u32IndexJ  ]) >> 1 >> ( u8PixelBits - 8 ); 
				pu8DstBmpData[(u32TmpHeight - u32IndexI)*u32TmpWidth * 3 + (u32TmpWidth - u32IndexJ) * 3 + 1] =     pu16ResRawData[ u32IndexI   *u32TmpWidth + u32IndexJ  ]		  >> ( u8PixelBits - 8 ); 
				pu8DstBmpData[(u32TmpHeight - u32IndexI)*u32TmpWidth * 3 + (u32TmpWidth - u32IndexJ) * 3 + 2] = (   pu16ResRawData[ u32IndexI   *u32TmpWidth + u32IndexJ+1]
																												  + pu16ResRawData[ u32IndexI   *u32TmpWidth + u32IndexJ-1]) >> 1 >> ( u8PixelBits - 8 ); 
			}
		}
	}
	else
	{
		InsertText( "Pixel bits error!", OUTPUT_RED );
	}
}
  
   
void CUSBTestDlg::FrameDisplay( Uint8* pu8Rgb32, Uint32 u32Width, Uint32 u32Height )
{
	CDC		*pcdDC;
	CRect	crRect;	
	CWnd	*pcwWnd = GetDlgItem( IDC_STATIC_DISPLAY );
	pcwWnd->GetClientRect( &crRect );
	pcdDC = pcwWnd->GetDC();

	BITMAPINFOHEADER bmiHeader;
	bmiHeader.biSize		  = sizeof(BITMAPINFOHEADER);
	bmiHeader.biWidth		  = u32Width;
	bmiHeader.biHeight		  = u32Height;
	bmiHeader.biPlanes		  = 1;
	bmiHeader.biBitCount	  = 24;
	bmiHeader.biCompression	  = BI_RGB;
	bmiHeader.biSizeImage	  = 0;
	bmiHeader.biXPelsPerMeter = 0;
	bmiHeader.biYPelsPerMeter = 0;
	bmiHeader.biClrUsed		  = 0;
	bmiHeader.biClrImportant  = 0;

	Uint32 u32DispWidth  = min( u32Width,  640 * m_pixScale );
	Uint32 u32DispHeight = min( u32Height, 480 * m_pixScale );
	
	SetStretchBltMode( pcdDC->m_hDC, HALFTONE );

	StretchDIBits( pcdDC->m_hDC, 
				  crRect.left, crRect.top, crRect.left+u32DispWidth/m_pixScale, crRect.top+u32DispHeight/m_pixScale, 
				  m_s32locStartX, m_s32locStartY, u32DispWidth, u32DispHeight, 
				  pu8Rgb32, ( BITMAPINFO* )&bmiHeader, BI_RGB, SRCCOPY );	

	ReleaseDC( pcdDC );
}
   

void CUSBTestDlg::CaptureFrame( void )
{
	Uint32 u32TmpState;

	ArduCam_beginCaptureImage( m_cUsbCameraHd );
	
	while ( m_u32FrameCaptureThreadEn == THREAD_ENABLE )
	{
		u32TmpState = ArduCam_captureImage( m_cUsbCameraHd );
		
		if ( u32TmpState < 255 )
		{
			m_u32FrameCaptureCnt = m_u32FrameCaptureCnt + u32TmpState;
		}
		else
		{
			DispErrorLog( u32TmpState );
		}
	}

	ArduCam_endCaptureImage( m_cUsbCameraHd );

	m_u32CaptFlag = 0;

	return;
}
  

void CUSBTestDlg::ReadFrame( void )
{
	Uint32 u32TmpState;

	Uint8*		pu8TmpFrameData = NULL;
	ArduCamCfg	stTmpFramePara;
	ArduIMUData* pstTmpIMUData; 

	while ( m_u32FrameReadThreadEn == THREAD_ENABLE )
	{
		if ( ArduCam_availiableImage( m_cUsbCameraHd ) > 0 )
		{
			u32TmpState = ArduCam_readImageAndIMU( m_cUsbCameraHd, pu8TmpFrameData, pstTmpIMUData );
			ArduCam_getSensorCfg( m_cUsbCameraHd, &stTmpFramePara );
			
			if ( u32TmpState == USB_CAMERA_NO_ERROR )
			{
				m_u32FrameReadCnt++;

				//Uint8* pu8OutRgb24 = ( Uint8* )malloc( stTmpFramePara.u32Width * stTmpFramePara.u32Height * 3 + 64 );
				
				Raw2rgb24( m_pu8OutRgb24, pu8TmpFrameData, stTmpFramePara.u32Width, stTmpFramePara.u32Height, m_u32RawMode, stTmpFramePara.u8PixelBits );

				

				if (m_u32ShotFlag)
				{
					CFile m_pImageRAWFile = new CFile();
					CFile m_pImageBMPFile = new CFile();
					CString strName;
					static int iFileIndex = 1;

					strName.Format("Image_%d.raw",iFileIndex);
					
					if(m_pImageRAWFile.Open(strName,CFile::modeCreate|CFile::modeWrite))
					{
						m_pImageRAWFile.Write(pu8TmpFrameData, stTmpFramePara.u32Width * stTmpFramePara.u32Height);
						m_pImageRAWFile.Close();
					}

					strName.Format("Image_%d.bmp",iFileIndex);
					
					if(m_pImageBMPFile.Open(strName,CFile::modeCreate|CFile::modeWrite))
					{
						BITMAPFILEHEADER bmfHdr;
						BITMAPINFOHEADER bmiHeader;

						bmiHeader.biSize		  = sizeof(BITMAPINFOHEADER);
						bmiHeader.biWidth		  = stTmpFramePara.u32Width;
						bmiHeader.biHeight		  = stTmpFramePara.u32Height;
						bmiHeader.biPlanes		  = 1;
						bmiHeader.biBitCount	  = 24;
						bmiHeader.biCompression	  = BI_RGB;
						bmiHeader.biSizeImage	  = 0;
						bmiHeader.biXPelsPerMeter = 0;
						bmiHeader.biYPelsPerMeter = 0;
						bmiHeader.biClrUsed		  = 0;
						bmiHeader.biClrImportant  = 0;

						bmfHdr.bfType = 0x4D42;     // 文件类型: "BM"  
						bmfHdr.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + stTmpFramePara.u32Width * stTmpFramePara.u32Height * 3;  // 位图文件大小  
						bmfHdr.bfReserved1 = 0;  
						bmfHdr.bfReserved2 = 0;  
						bmfHdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER) ;  

						// write the BITMAPFILEHEADER  
						  
						m_pImageBMPFile.Write(&bmfHdr, sizeof(BITMAPFILEHEADER));  
						
						// write the BITMAPINFOHEADER  
						m_pImageBMPFile.Write(&bmiHeader, sizeof(BITMAPINFOHEADER));  
						
						// write the bmp data  
						
						m_pImageBMPFile.Write(m_pu8OutRgb24, stTmpFramePara.u32Width * stTmpFramePara.u32Height * 3);
						m_pImageBMPFile.Close();
					}

					++iFileIndex;
					m_u32ShotFlag = 0;

					

				}

				FrameDisplay( m_pu8OutRgb24, stTmpFramePara.u32Width, stTmpFramePara.u32Height );

				//if ( pu8OutRgb24 != NULL )		free( pu8OutRgb24 );
				
				/*CString csTmpString;

				csTmpString.Format( "索 引 号：0x%x", pstTmpIMUData->u16Index );
				m_sttIMUIdx.SetWindowText( csTmpString );
				csTmpString.Format( "时 间 戳：0x%x", pstTmpIMUData->u32TimeStamp );
				m_sttIMUTime.SetWindowText( csTmpString );
				csTmpString.Format( "加速度 X：0x%x", pstTmpIMUData->u16AccX );
				m_sttIMUAccX.SetWindowText( csTmpString );
				csTmpString.Format( "加速度 Y：0x%x", pstTmpIMUData->u16AccY );
				m_sttIMUAccY.SetWindowText( csTmpString );
				csTmpString.Format( "加速度 Z：0x%x", pstTmpIMUData->u16AccZ );
				m_sttIMUAccZ.SetWindowText( csTmpString );
				csTmpString.Format( "姿态角 X：0x%x", pstTmpIMUData->u16GyroX );
				m_sttIMUGyroX.SetWindowText( csTmpString );
				csTmpString.Format( "姿态角 Y：0x%x", pstTmpIMUData->u16GyroY );
				m_sttIMUGyroY.SetWindowText( csTmpString );
				csTmpString.Format( "姿态角 Z：0x%x", pstTmpIMUData->u16GyroZ );
				m_sttIMUGyroZ.SetWindowText( csTmpString );
				csTmpString.Format( "温    度：0x%x", pstTmpIMUData->u16Temp );
				m_sttIMUTemp.SetWindowText( csTmpString );*/

				ArduCam_del( m_cUsbCameraHd );
			} 
			else
			{
				DispErrorLog( u32TmpState );
			}			
		}
		else
		{
			//Sleep( 1 );
		}
	}
				
	return;
}
  

void CUSBTestDlg::CaptureIMU( void )
{
	while ( m_u32IMUCaptureThreadEn == THREAD_ENABLE )
	{
		ArduCam_captureIMU( m_cUsbCameraHd );
	}
	
	return;
}
  

void CUSBTestDlg::OnBnClickedButtonScan()
{
	// TODO: 在此添加控件通知处理程序代码
	CString	csTmpString;

	Uint32 u32UsbIndexArray[100];
	Uint16 u16TmpVID = 0;

	switch ( m_cmbVID.GetCurSel() )
	{
		case 0	: u16TmpVID = ARDUCAM_VID;			break;
		default : u16TmpVID = ARDUCAM_VID;			break;
	}
	
	Uint32 u32UsbNum = ArduCam_scan( u32UsbIndexArray, u16TmpVID );

	m_cmbUsbIndex.ResetContent();
	for ( Uint32 u32Index = 0; u32Index < u32UsbNum; u32Index++ )
	{
		csTmpString.Format( "%d", u32UsbIndexArray[u32Index] );
		m_cmbUsbIndex.AddString( csTmpString );
	}

	if ( u32UsbNum > 0 )		m_cmbUsbIndex.SetCurSel( 0 );
}
  

void CUSBTestDlg::OnBnClickedButtonOpen()
{
	// TODO: 在此添加控件通知处理程序代码
	CString	csTmpString;

	Int32 s32Index = m_cmbUsbIndex.GetCurSel();
	m_cmbUsbIndex.GetLBText( s32Index, csTmpString );
	Uint32 u32TmpUsbIndex = atoi( csTmpString );
	
	ArduCamCfg stTmpUsbCameraCfg;

	m_edtWidth.GetWindowText( csTmpString );
	stTmpUsbCameraCfg.u32Width = atoi( csTmpString );
	m_edtHeight.GetWindowText( csTmpString );
	stTmpUsbCameraCfg.u32Height = atoi( csTmpString );

	switch ( m_cmbBits.GetCurSel() )
	{
		case 0:		stTmpUsbCameraCfg.u8PixelBytes = 1;		
					stTmpUsbCameraCfg.u8PixelBits  = 8;			break;
		case 1:		stTmpUsbCameraCfg.u8PixelBytes = 2;	
					stTmpUsbCameraCfg.u8PixelBits  = 10;		break;
		case 2:		stTmpUsbCameraCfg.u8PixelBytes = 2;	
					stTmpUsbCameraCfg.u8PixelBits  = 12;		break;
		case 3:		stTmpUsbCameraCfg.u8PixelBytes = 2;	
					stTmpUsbCameraCfg.u8PixelBits  = 14;		break;
		case 4:		stTmpUsbCameraCfg.u8PixelBytes = 2;	
					stTmpUsbCameraCfg.u8PixelBits  = 16;		break;
		default:	stTmpUsbCameraCfg.u8PixelBytes = 1;		
					stTmpUsbCameraCfg.u8PixelBits  = 8;			break;
	}

	switch ( m_cmbI2cMode.GetCurSel() )
	{
		case 0:		stTmpUsbCameraCfg.emI2cMode = I2C_MODE_8_8;			break;
		case 1:		stTmpUsbCameraCfg.emI2cMode = I2C_MODE_8_16;		break;
		case 2:		stTmpUsbCameraCfg.emI2cMode = I2C_MODE_16_8;		break;
		case 3:		stTmpUsbCameraCfg.emI2cMode = I2C_MODE_16_16;		break;
		default:	stTmpUsbCameraCfg.emI2cMode = I2C_MODE_8_16;		break;
	}

	m_edtShipAddr.GetWindowText( csTmpString );
	stTmpUsbCameraCfg.u32SensorShipAddr = atoi( csTmpString );

	switch ( m_cmbType.GetCurSel() )
	{
		case 0:		stTmpUsbCameraCfg.u16Vid  = ARDUCAM_VID;			break;
		default:	stTmpUsbCameraCfg.u16Vid  = ARDUCAM_VID;			break;
	}

	Uint32 u32Value = ArduCam_open( m_cUsbCameraHd, &stTmpUsbCameraCfg, u32TmpUsbIndex );

	DispErrorLog( u32Value );

	if ( u32Value == USB_CAMERA_NO_ERROR )
	{
		m_btPlay.EnableWindow( TRUE );
		m_btStop.EnableWindow( TRUE );
		m_btClose.EnableWindow( TRUE );
		m_btShot.EnableWindow( TRUE );
		m_btInit.EnableWindow( FALSE );
		m_btRegRead.EnableWindow( TRUE );
		m_btRegWrite.EnableWindow( TRUE );

		m_chkForceDisp.EnableWindow( TRUE );

		m_u32FrameReadThreadEn = THREAD_ENABLE;												
		m_ptdFrameReadThread   = AfxBeginThread( _FrameReadThread, this );
		SetThreadAffinityMask( m_ptdFrameReadThread->m_hThread, 0x44 );

		InsertText( "USB camera init success!", OUTPUT_BLUE );
	}
}
  

void CUSBTestDlg::OnBnClickedButtonInit()
{
	// TODO: 在此添加控件通知处理程序代码
	ArduCamCfg stTmpUsbCameraCfg;
	CString	csTmpString;

	m_edtWidth.GetWindowText( csTmpString );
	stTmpUsbCameraCfg.u32Width = atoi( csTmpString );
	m_edtHeight.GetWindowText( csTmpString );
	stTmpUsbCameraCfg.u32Height = atoi( csTmpString );

	switch ( m_cmbBits.GetCurSel() )
	{
		case 0:		stTmpUsbCameraCfg.u8PixelBytes = 1;		
					stTmpUsbCameraCfg.u8PixelBits  = 8;			break;
		case 1:		stTmpUsbCameraCfg.u8PixelBytes = 2;	
					stTmpUsbCameraCfg.u8PixelBits  = 10;		break;
		case 2:		stTmpUsbCameraCfg.u8PixelBytes = 2;	
					stTmpUsbCameraCfg.u8PixelBits  = 12;		break;
		case 3:		stTmpUsbCameraCfg.u8PixelBytes = 2;	
					stTmpUsbCameraCfg.u8PixelBits  = 14;		break;
		case 4:		stTmpUsbCameraCfg.u8PixelBytes = 2;	
					stTmpUsbCameraCfg.u8PixelBits  = 16;		break;
		default:	stTmpUsbCameraCfg.u8PixelBytes = 1;		
					stTmpUsbCameraCfg.u8PixelBits  = 8;			break;
	}

	switch ( m_cmbI2cMode.GetCurSel() )
	{
		case 0:		stTmpUsbCameraCfg.emI2cMode = I2C_MODE_8_8;			break;
		case 1:		stTmpUsbCameraCfg.emI2cMode = I2C_MODE_8_16;		break;
		case 2:		stTmpUsbCameraCfg.emI2cMode = I2C_MODE_16_8;		break;
		case 3:		stTmpUsbCameraCfg.emI2cMode = I2C_MODE_16_16;		break;
		default:	stTmpUsbCameraCfg.emI2cMode = I2C_MODE_8_16;		break;
	}

	m_edtShipAddr.GetWindowText( csTmpString );
	stTmpUsbCameraCfg.u32SensorShipAddr = atoi( csTmpString );

	switch ( m_cmbType.GetCurSel() )
	{
		case 0:		stTmpUsbCameraCfg.u16Vid  = ARDUCAM_VID;	break;
		default:	stTmpUsbCameraCfg.u16Vid  = ARDUCAM_VID;	break;
	}

	Uint32 u32Value = ArduCam_autoopen( m_cUsbCameraHd, &stTmpUsbCameraCfg );

	DispErrorLog( u32Value );

	if ( u32Value == USB_CAMERA_NO_ERROR )
	{
		m_btPlay.EnableWindow( TRUE );
		m_btStop.EnableWindow( TRUE );
		m_btClose.EnableWindow( TRUE );
		m_btInit.EnableWindow( FALSE );
		m_btShot.EnableWindow( TRUE );
		m_btRegRead.EnableWindow( TRUE );
		m_btRegWrite.EnableWindow( TRUE );

		m_chkForceDisp.EnableWindow( TRUE );

		m_u32FrameReadThreadEn = THREAD_ENABLE;												
		m_ptdFrameReadThread   = AfxBeginThread( _FrameReadThread, this );
		SetThreadAffinityMask( m_ptdFrameReadThread->m_hThread, 0x44 );

		InsertText( "USB camera init success!", OUTPUT_BLUE );
		ArduCam_writeSensorReg( m_cUsbCameraHd, 0x07, 920 );
	}
	int i = 0;
	do //while( (reg_setting[i].sensor_addr != 0xffff) || (reg_setting[i].sensor_value != 0xffff));
	{
		ArduCam_writeSensorReg( m_cUsbCameraHd, reg_setting[i].sensor_addr, reg_setting[i].sensor_value );
		i++;
	}while( (reg_setting[i].sensor_addr != 0xffff) || (reg_setting[i].sensor_value != 0xffff));
	
	m_cmbSensor.EnableWindow(FALSE);
	m_edtWidth.EnableWindow(FALSE);
	m_edtHeight.EnableWindow(FALSE);
	m_cmbI2cMode.EnableWindow(FALSE);
	m_cmbBits.EnableWindow(FALSE);
	m_edtShipAddr.EnableWindow(FALSE);
	//m_btRawMode.EnableWindow(FALSE);
}
  

void CUSBTestDlg::OnBnClickedButtonPlay()
{
	// TODO: 在此添加控件通知处理程序代码
	if ( m_u8PlayState != STATE_PLAY )
	{
		//ArduCam_writeSensorReg( m_cUsbCameraHd, 0x07, 904 );
		
		m_u8PlayState = STATE_PLAY;
		
		m_u32FrameCaptureThreadEn = THREAD_ENABLE;												
		m_ptdFrameCaptureThread   = AfxBeginThread( _FrameCaptureThread, this );
		SetThreadAffinityMask( m_ptdFrameCaptureThread->m_hThread, 0x22 );

		/*m_u32IMUCaptureThreadEn = THREAD_ENABLE;												
		m_ptdIMUCaptureThread   = AfxBeginThread( _IMUCaptureThread, this );
		SetThreadAffinityMask( m_ptdIMUCaptureThread->m_hThread, 0x11 );*/

		//GetDlgItem( IDOK )->EnableWindow( FALSE );
		m_btPlay.EnableWindow( FALSE );
		m_btStop.EnableWindow( TRUE );
		m_btClose.EnableWindow( FALSE );
		//m_btShot.EnableWindow( FALSE );

		
	}
}
  

void CUSBTestDlg::OnBnClickedButtonStop()
{
	// TODO: 在此添加控件通知处理程序代码
	if ( m_u8PlayState != STATE_STOP )
	{
		ArduCam_writeSensorReg( m_cUsbCameraHd, 0x07, 920 );
		
		m_u8PlayState = STATE_STOP;

		m_u32CaptFlag = 1;
			
		m_u32FrameCaptureThreadEn = THREAD_DISABLE;												
		//m_u32FrameReadThreadEn    = THREAD_DISABLE;	

		m_u32IMUCaptureThreadEn = THREAD_DISABLE;	
		
		m_u32FrameCaptureCnt = 0;
		//m_u32FrameReadCnt    = 0;

		m_u32FrameCaptureCntBak = 0;
		//m_u32FrameReadCntBak    = 0;

		//GetDlgItem( IDOK )->EnableWindow( TRUE );
		/*
		while ( m_u32CaptFlag > 0 )
		{			
			m_u32CaptFlag++;
			Sleep(100);
			
			if ( m_u32CaptFlag > 50 )
			{
				m_u32CaptFlag = 0;
			}
		}*/

		m_btPlay.EnableWindow( TRUE );
		m_btStop.EnableWindow( FALSE );
		m_btClose.EnableWindow( TRUE );
		m_btShot.EnableWindow( TRUE );

		
	}
}
  

void CUSBTestDlg::OnBnClickedButtonShot()
{
	// TODO: 在此添加控件通知处理程序代码
	m_u32ShotFlag  = 1;


} 
  

void CUSBTestDlg::OnBnClickedButtonClose()
{
	// TODO: 在此添加控件通知处理程序代码
	m_u32FrameReadThreadEn  = THREAD_DISABLE;
	m_u32FrameReadCnt		= 0;
	m_u32FrameReadCntBak    = 0;
	
	ArduCam_close( m_cUsbCameraHd );

	m_btPlay.EnableWindow( FALSE );
	m_btStop.EnableWindow( FALSE );
	m_btClose.EnableWindow( FALSE );
	m_btShot.EnableWindow( FALSE );
	m_btInit.EnableWindow( TRUE );
	m_btRegRead.EnableWindow( FALSE );
	m_btRegWrite.EnableWindow( FALSE );

	m_chkForceDisp.EnableWindow( FALSE );

	m_edtWidth.EnableWindow(TRUE);
	m_edtHeight.EnableWindow(TRUE);
	m_cmbI2cMode.EnableWindow(TRUE);
	m_cmbBits.EnableWindow(TRUE);
	m_edtShipAddr.EnableWindow(TRUE);
	m_btRawMode.EnableWindow(TRUE);
	m_cmbSensor.EnableWindow(TRUE);
}
 

void CUSBTestDlg::OnBnClickedButtonRegRead()
{
	// TODO: 在此添加控件通知处理程序代码
	CString csTmpString;
	Uint32 u32TmpAddr;
	Uint32 u32TmpValue;

	m_edtRegAddr.GetWindowText( csTmpString );
	u32TmpAddr = atoi( csTmpString );

	Uint32 u32State = ArduCam_readSensorReg( m_cUsbCameraHd, u32TmpAddr, &u32TmpValue );

	if ( u32State == USB_CAMERA_NO_ERROR )		csTmpString.Format( "%d", u32TmpValue );
	else										csTmpString.Format( "error!" );

	m_edtRegValue.SetWindowText( csTmpString );
}
 

void CUSBTestDlg::OnBnClickedButtonRegWrite()
{
	// TODO: 在此添加控件通知处理程序代码
	CString csTmpString;
	Uint32 u32TmpAddr;
	Uint32 u32TmpValue;

	m_edtRegAddr.GetWindowText( csTmpString );
	u32TmpAddr = atoi( csTmpString );

	m_edtRegValue.GetWindowText( csTmpString );
	u32TmpValue = atoi( csTmpString );

	Uint32 u32State = ArduCam_writeSensorReg( m_cUsbCameraHd, u32TmpAddr, u32TmpValue );

	if ( u32State != USB_CAMERA_NO_ERROR )		csTmpString.Format( "error!" );
	m_edtRegValue.SetWindowText( csTmpString );
}
  

void CUSBTestDlg::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
	if ( m_u8PlayState != STATE_STOP )
	{
		m_u8PlayState = STATE_STOP;

		m_u32FrameCaptureThreadEn = THREAD_DISABLE;												
		m_u32FrameReadThreadEn    = THREAD_DISABLE;	
		
		m_u32IMUCaptureThreadEn = THREAD_DISABLE;	
	}

	Sleep(500);
}


void CUSBTestDlg::OnBnClickedButtonByteConv()
{
	// TODO: 在此添加控件通知处理程序代码
	if ( m_u32RawMode == RAW_RG )
	{
		m_u32RawMode = RAW_GR;
		m_btRawMode.SetWindowTextA( "RAWMode(GR)" );
	}
	else if ( m_u32RawMode == RAW_GR )
	{
		m_u32RawMode = RAW_GB;
		m_btRawMode.SetWindowTextA( "RAWMode(GB)" );
	}
	else if ( m_u32RawMode == RAW_GB )
	{
		m_u32RawMode = RAW_BG;
		m_btRawMode.SetWindowTextA( "RAWMode(BG)" );
	}
	else
	{
		m_u32RawMode = RAW_RG;
		m_btRawMode.SetWindowTextA( "RAWMode(RG)" );
	}
}
  

void CUSBTestDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值	
	/* ----- 如鼠标位置在显示区域内，读取当前鼠标位置，并保存，用于鼠标弹起时拖动处理	----- */
	/* ----- 如鼠标位置不在显示区域内，将当前鼠标位置置为（0,0），不做拖动处理			----- */
	if (    ( point.x >= m_u32FrameLeftloc  )
		 && ( point.x <= m_u32FrameRightloc ) 
		 && ( point.y >= m_u32FrameUploc    )
		 && ( point.y <= m_u32FrameDownloc  ) )
	{
		if ( m_u8PlayState != STATE_PLAY )		return;
		m_s32LocXMoved = point.x;
		m_s32LocYMoved = point.y;

		m_sttDisplay.SetFocus();
	}
	else
	{
		m_s32LocXMoved = 0xFFFF;
		m_s32LocYMoved = 0xFFFF;
	}

	

	CDialog::OnLButtonDown(nFlags, point);
}
  

void CUSBTestDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (    ( point.x >= m_u32FrameLeftloc  )
		 && ( point.x <= m_u32FrameRightloc ) 
		 && ( point.y >= m_u32FrameUploc    )
		 && ( point.y <= m_u32FrameDownloc  ) )
	{
		if ( m_u8PlayState != STATE_PLAY )		return;
		ArduCamCfg	stTmpFramePara;
		ArduCam_getSensorCfg( m_cUsbCameraHd, &stTmpFramePara );
		
		m_s32LocXMoved = point.x - m_s32LocXMoved;
		m_s32LocYMoved = point.y - m_s32LocYMoved;

		m_s32locStartX = m_s32locStartX - ( m_s32LocXMoved * m_pixScale );
		m_s32locStartY = m_s32locStartY + ( m_s32LocYMoved * m_pixScale );

		if ( ( m_s32locStartX + 640 * m_pixScale ) > stTmpFramePara.u32Width )
		{
			m_s32locStartX = stTmpFramePara.u32Width - 640 * m_pixScale;
		}

		if ( ( m_s32locStartY + 480 * m_pixScale ) > stTmpFramePara.u32Height )
		{
			m_s32locStartY = stTmpFramePara.u32Height - 480 * m_pixScale;
		}

		m_s32locStartX = max( 0, m_s32locStartX );
		m_s32locStartY = max( 0, m_s32locStartY );

		FrameDisplay( m_pu8OutRgb24, stTmpFramePara.u32Width, stTmpFramePara.u32Height );
	}
	else
	{
		m_s32LocXMoved = 0xFFFF;
		m_s32LocYMoved = 0xFFFF;
	}

	CDialog::OnLButtonDown(nFlags, point);
	CDialog::OnLButtonUp(nFlags, point);
}


BOOL CUSBTestDlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	/* ----- 将屏幕位置转换为控件内的位置 ----- */
	ScreenToClient( &pt );

	/* ----- 如鼠标位置在显示区域内，读取滚轮转动的值，调整显示范围的大小（增加边界判断） ----- */
	if (    ( pt.x >= m_u32FrameLeftloc  )
		 && ( pt.x <= m_u32FrameRightloc ) 
		 && ( pt.y >= m_u32FrameUploc    )
		 && ( pt.y <= m_u32FrameDownloc  ) )
	{	
		if ( m_u8PlayState != STATE_PLAY )		return CDialog::OnMouseWheel(nFlags, zDelta, pt);
		ArduCamCfg	stTmpFramePara;
		ArduCam_getSensorCfg( m_cUsbCameraHd, &stTmpFramePara );
		
		if ( m_s32ScaleIndex > zDelta / 120 )
		{
			m_s32ScaleIndex = min( 17, m_s32ScaleIndex - zDelta / 120 );
		}
		else
		{
			m_s32ScaleIndex = 0;
		}

		switch ( m_s32ScaleIndex )
		{
			case 17 :	m_pixScale = 10.0;		break;
			case 16 :	m_pixScale = 8.0;		break;
			case 15 :	m_pixScale = 6.0;		break;
			case 14 :	m_pixScale = 5.0;		break;
			case 13 :	m_pixScale = 4.0;		break;
			case 12 :	m_pixScale = 3.0;		break;
			case 11 :	m_pixScale = 2.0;		break;
			case 10 :	m_pixScale = 1.0;		break;
			case 9 :	m_pixScale = 0.9;		break;
			case 8 :	m_pixScale = 0.8;		break;
			case 7 :	m_pixScale = 0.7;		break;
			case 6 :	m_pixScale = 0.6;		break;
			case 5 :	m_pixScale = 0.5;		break;
			case 4 :	m_pixScale = 0.4;		break;
			case 3 :	m_pixScale = 0.3;		break;
			case 2 :	m_pixScale = 0.2;		break;
			case 1 :	m_pixScale = 0.1;		break;
			case 0 :	m_pixScale = 0.05;		break;
			default :	m_pixScale = 1.0;		break;	
		}

		if ( ( m_s32locStartX + 640 * m_pixScale ) > stTmpFramePara.u32Width )
		{
			m_s32locStartX = stTmpFramePara.u32Width - 640 * m_pixScale;
		}

		if ( ( m_s32locStartY + 480 * m_pixScale ) > stTmpFramePara.u32Height )
		{
			m_s32locStartY = stTmpFramePara.u32Height - 480 * m_pixScale;
		}

		m_s32locStartX = max( 0, m_s32locStartX );
		m_s32locStartY = max( 0, m_s32locStartY );
		
		/* ----- 绘制黑色背景 ----- */
		CDC		*pcdDC;
		CRect	crRect;	
		CWnd	*pcwWnd = GetDlgItem( IDC_STATIC_DISPLAY );
		pcwWnd->GetClientRect( &crRect );
		pcdDC = pcwWnd->GetDC();

		BITMAPINFOHEADER bmiHeader;
		bmiHeader.biSize		  = sizeof(BITMAPINFOHEADER);
		bmiHeader.biWidth		  = 640;
		bmiHeader.biHeight		  = 480;
		bmiHeader.biPlanes		  = 1;
		bmiHeader.biBitCount	  = 24;
		bmiHeader.biCompression	  = BI_RGB;
		bmiHeader.biSizeImage	  = 0;
		bmiHeader.biXPelsPerMeter = 0;
		bmiHeader.biYPelsPerMeter = 0;
		bmiHeader.biClrUsed		  = 0;
		bmiHeader.biClrImportant  = 0;		
		SetStretchBltMode( pcdDC->m_hDC, HALFTONE );
		StretchDIBits( pcdDC->m_hDC, 
					   crRect.left, crRect.top, crRect.left+640, crRect.top+480, 
					   0, 0, 640, 480, 
					   NULL, ( BITMAPINFO* )&bmiHeader, BI_RGB, BLACKNESS );	
		/* ----- 绘制黑色背景 ----- */

		FrameDisplay( m_pu8OutRgb24, stTmpFramePara.u32Width, stTmpFramePara.u32Height );
	}

	return CDialog::OnMouseWheel(nFlags, zDelta, pt);
}


void CUSBTestDlg::OnBnClickedCheck1()
{
	// TODO: 在此添加控件通知处理程序代码
	if ( m_cUsbCameraHd != NULL )
	{
		if ( m_chkForceDisp.GetCheck() == TRUE )
		{
			ArduCam_enableForceRead( m_cUsbCameraHd );
		}
		else
		{
			ArduCam_disableForceRead( m_cUsbCameraHd );
		}
	}
}

void CUSBTestDlg::OnCbnSelchangeCombo6()
{
	// TODO: 在此添加控件通知处理程序代码
	switch(m_cmbSensor.GetCurSel())
	{
	case OV7670: 
		m_edtWidth.SetWindowText( "640" );
		m_edtHeight.SetWindowText( "480" );
		m_cmbI2cMode.SetCurSel(0);
		m_edtShipAddr.SetWindowText( "66" );

		m_u32RawMode = RAW_RG;								// 默认RG模式
		m_btRawMode.SetWindowTextA( "RAWMode(RG)" );
		reg_setting = (sensor_reg*) &reg_setting_OV7670[0];
		break;
	case OV7725:
		m_edtWidth.SetWindowText( "640" );
		m_edtHeight.SetWindowText( "480" );
		m_cmbI2cMode.SetCurSel(0);
		m_edtShipAddr.SetWindowText( "66" );

		m_u32RawMode = RAW_RG;								// 默认RG模式
		m_btRawMode.SetWindowTextA( "RAWMode(RG)" );
		reg_setting = (sensor_reg*) &reg_setting_OV7725[0];
		break;
	case MT9V034:
		m_edtWidth.SetWindowText( "640" );
		m_edtHeight.SetWindowText( "480" );
		m_cmbI2cMode.SetCurSel(1);
		m_edtShipAddr.SetWindowText( "144" );

		m_u32RawMode = RAW_RG;								// 默认RG模式
		m_btRawMode.SetWindowTextA( "RAWMode(RG)" );
		reg_setting = (sensor_reg*) &reg_setting_MT9V034[0];
		break;
	case AR0134:
		m_edtWidth.SetWindowText( "1280" );
		m_edtHeight.SetWindowText( "960" );
		m_cmbI2cMode.SetCurSel(3);
		m_edtShipAddr.SetWindowText( "32" );

		m_u32RawMode = RAW_GB;								// 默认RG模式
		m_btRawMode.SetWindowTextA( "RAWMode(GB)" );
		reg_setting = (sensor_reg*) &reg_setting_AR0134[0];
		break;
	case MT9J001:
		m_edtWidth.SetWindowText( "3664" );
		m_edtHeight.SetWindowText( "2748" );
		m_cmbI2cMode.SetCurSel(3);
		m_edtShipAddr.SetWindowText( "32" );

		m_u32RawMode = RAW_RG;								// 默认RG模式
		m_btRawMode.SetWindowTextA( "RAWMode(RG)" );
		reg_setting = (sensor_reg*) &reg_setting_MT9J001[0];
		break;
	case MT9F002:
		m_edtWidth.SetWindowText( "4384" );//4384
		m_edtHeight.SetWindowText( "3288" );//3288
		m_cmbI2cMode.SetCurSel(3);
		m_edtShipAddr.SetWindowText( "32" );

		m_u32RawMode = RAW_BG;								// 默认RG模式
		m_btRawMode.SetWindowTextA( "RAWMode(BG)" );
		reg_setting = (sensor_reg*) &reg_setting_MT9F002[0];
		break;
	case MT9N001:
		m_edtWidth.SetWindowText( "3488" );
		m_edtHeight.SetWindowText( "2616" );
		m_cmbI2cMode.SetCurSel(3);
		m_edtShipAddr.SetWindowText( "32" );

		m_u32RawMode = RAW_BG;								// 默认RG模式
		m_btRawMode.SetWindowTextA( "RAWMode(BG)" );
		reg_setting = (sensor_reg*) &reg_setting_MT9N001[0];
		break;

	
	default:
		break;
	}
	

}
