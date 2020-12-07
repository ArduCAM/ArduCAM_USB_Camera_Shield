// USBTestDlg.cpp : 
//

#include "stdafx.h"
#include "USBTest.h"
#include "USBTestDlg.h"
#include "AboutDlg.h"
#include "ControlDlg.h"
#include "DataInfoDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


UINT _FrameCaptureThread( LPVOID lParam );						
UINT _FrameReadThread( LPVOID lParam );								

CUSBTestDlg::CUSBTestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CUSBTestDlg::IDD, pParent)
	, m_rdoVertical(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_cUsbCameraHd       = NULL;
	m_cUsbCameraCfgHd    = NULL;
	m_cUsbCameraRecordHd = NULL;

	m_ControlDlg = NULL;

	GetModuleFileName( NULL, m_csCfgFileName.GetBuffer(512), 512 );									
	m_csCfgFileName.ReleaseBuffer();
	Int32 s32TmpPathSize = m_csCfgFileName.ReverseFind( '\\' );	
	m_csCfgFileName	= m_csCfgFileName.Left( s32TmpPathSize );	

	m_csCfgFileName = m_csCfgFileName + _T("\\Config\\*.cfg");
	
	m_pu8OutRgb24 = ( Uint8* )malloc( 4096 * 4096 * 3 * 2 );

	GetModuleFileName( NULL, m_csShotPath.GetBuffer(512), 512 );
	m_csShotPath.ReleaseBuffer();
	s32TmpPathSize = m_csShotPath.ReverseFind( '\\' );
	m_csShotPath = m_csShotPath.Left( s32TmpPathSize ) + "\\Shot";
	if ( !PathIsDirectory( m_csShotPath ) )	CreateDirectory( m_csShotPath, NULL ); 

	GetModuleFileName( NULL, m_csRecordPath.GetBuffer(512), 512 );
	m_csRecordPath.ReleaseBuffer();
	s32TmpPathSize = m_csRecordPath.ReverseFind( '\\' );
	m_csRecordPath = m_csRecordPath.Left( s32TmpPathSize ) + "\\Record";
	if ( !PathIsDirectory( m_csRecordPath ) )	CreateDirectory( m_csRecordPath, NULL );

	m_csRecordSubPath.Format( "\\Test" );

	m_u32TransLvl = 0;

	m_u32ShotTimeFlag = 0;

	m_u32DisplayWidth  = 640;
	m_u32DisplayHeight = 480;
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
	DDX_Control(pDX, IDC_EDIT2, m_edtHeight);
	DDX_Control(pDX, IDC_COMBO1, m_cmbBits);
	DDX_Control(pDX, IDC_BUTTON_SCAN, m_btScan);
	DDX_Control(pDX, IDC_COMBO3, m_cmbUsbIndex);
	DDX_Control(pDX, IDC_BUTTON_OPEN, m_btOpen);
	DDX_Control(pDX, IDC_BUTTON_CLOSE, m_btClose);
	DDX_Control(pDX, IDC_BUTTON_BYTE_CONV, m_btRawMode);
	DDX_Control(pDX, IDC_COMBO5, m_cmbI2cMode);
	DDX_Control(pDX, IDC_EDIT3, m_edtI2cAddr);
	DDX_Control(pDX, IDC_BUTTON_SHOT, m_btShot);
	DDX_Control(pDX, IDC_CHECK1, m_chkForceDisp);
	DDX_Control(pDX, IDC_COMBO_FORMAT, m_cmbImgFormat);
	DDX_Control(pDX, IDC_BUTTON_LOAD, m_btCfgLoad);
	DDX_Control(pDX, IDC_COMBO6, m_cmbCfgFileName);
	DDX_Control(pDX, IDC_BUTTON_FRASH, m_btCfgFrash);
	DDX_Control(pDX, IDC_EDIT_VID, m_edtVid);
	DDX_Control(pDX, IDC_EDIT13, m_edtWidth);
	DDX_Control(pDX, IDC_CHECK_FULL_SCREEN, m_chkFullScreen);
	DDX_Control(pDX, IDC_EDIT6, m_edtType);
	DDX_Control(pDX, IDC_CHECK_IRCUT, m_chkIRcut);
	DDX_Control(pDX, IDC_STATIC_UUID, m_sttSerial);
	DDX_Control(pDX, IDC_RADIO1, m_rdoHorizontal);
	DDX_Control(pDX, IDC_CHECK_WHITE_BALANCE, m_chkWhiteBalance);
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
	ON_WM_MOUSEMOVE ()

#ifdef FORCE_DISPLAY
	ON_BN_CLICKED(IDC_CHECK1, &CUSBTestDlg::OnBnClickedCheck1)
#endif

	ON_BN_CLICKED(IDC_BUTTON_LOAD, &CUSBTestDlg::OnBnClickedButtonLoad)
	ON_BN_CLICKED(IDC_BUTTON_FRASH, &CUSBTestDlg::OnBnClickedButtonRefresh)
	ON_BN_CLICKED(IDC_CHECK_FULL_SCREEN, &CUSBTestDlg::OnBnClickedCheckFullScreen)
	ON_BN_CLICKED(IDC_RADIO_DISPLAY, &CUSBTestDlg::OnBnClickedRadioDisplay)
	ON_BN_CLICKED(IDC_RADIO_SAVE_DATA, &CUSBTestDlg::OnBnClickedRadioSaveData)
	ON_BN_CLICKED(IDC_RADIO_SAVE_IMAGE, &CUSBTestDlg::OnBnClickedRadioSaveImage)
	ON_BN_CLICKED(IDC_RADIO_SAVE_VIDEO, &CUSBTestDlg::OnBnClickedRadioSaveVideo)
	ON_BN_CLICKED(IDC_CHECK_IRCUT, &CUSBTestDlg::OnBnClickedCheckIrcut)
	ON_BN_CLICKED(IDC_RADIO_MODE_HORIZONTAL, &CUSBTestDlg::OnBnClickedRadioModeHorizontal)
	ON_BN_CLICKED(IDC_RADIO_MODE_VERTICAL, &CUSBTestDlg::OnBnClickedRadioModeVertical)
	//ON_BN_CLICKED(IDC_CHECK_WHITE_BALANCE, &CUSBTestDlg::OnBnClickedCheckWhiteBalance)
	ON_BN_CLICKED(IDC_CHECK_WHITE_BALANCE, &CUSBTestDlg::OnBnClickedCheckWhiteBalance)
	ON_COMMAND(ID_MENU_ABOUT, &CUSBTestDlg::OnMenuAbout)
	ON_EN_CHANGE(IDC_EDIT4, &CUSBTestDlg::OnEnChangeEdit4)
	ON_BN_CLICKED(IDC_OPEN_FILE, &CUSBTestDlg::OnBnClickedOpenFile)
	ON_COMMAND(ID_FILE_OPEN32775, &CUSBTestDlg::OnBnClickedOpenFile)
	ON_MESSAGE(ID_MENU_SELECTALL, &CUSBTestDlg::OnSelectAll)
	ON_MESSAGE(ID_MENU_COPY, &CUSBTestDlg::OnCopy)
	ON_MESSAGE(ID_MENU_CLEAR, &CUSBTestDlg::OnClearAll)
	ON_COMMAND(ID_TOOLS_CONTROLS, &CUSBTestDlg::OnToolsControls)
	ON_COMMAND(ID_TOOLS_DATAINFO, &CUSBTestDlg::OnToolsDatainfo)
END_MESSAGE_MAP()



BOOL CUSBTestDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetIcon(m_hIcon, TRUE);			
	SetIcon(m_hIcon, FALSE);		

#ifndef FORCE_DISPLAY
	m_chkForceDisp.ShowWindow(FALSE);
#endif
	m_Menu.LoadMenu(IDR_MENU1);
	SetMenu(&m_Menu);
	m_tooltip.Create(&m_edtType);

	//将CToolTipCtrl与相应的控件对应起来
	m_tooltip.AddTool(&m_edtType, TTS_ALWAYSTIP);
	m_tooltip.SetDelayTime(300);
	m_chkFullScreen.SetCheck(TRUE);
	m_u32FrameCaptureThreadEn = THREAD_DISABLE;
	m_u32FrameReadThreadEn    = THREAD_DISABLE;

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

	( ( CButton* )GetDlgItem( IDC_RADIO_DISPLAY    ) )->EnableWindow( FALSE );
	( ( CButton* )GetDlgItem( IDC_RADIO_SAVE_DATA  ) )->EnableWindow( FALSE );
	( ( CButton* )GetDlgItem( IDC_RADIO_SAVE_IMAGE ) )->EnableWindow( FALSE );
	( ( CButton* )GetDlgItem( IDC_RADIO_SAVE_VIDEO ) )->EnableWindow( FALSE );
	
	( ( CButton* )GetDlgItem( IDC_RADIO_DISPLAY ) )->SetCheck( TRUE );
	m_u8RecordEn	 = RECORD_NULL;
	m_u8RecordEnd	 = RECORD_NULL;
	m_u32RecordIndex = 0;

	m_chkForceDisp.EnableWindow( FALSE );
	m_chkIRcut.EnableWindow( FALSE );

	m_chkWhiteBalance.EnableWindow( FALSE );

	m_u8DImageDispMode = MODE_HORIZONTAL;
	( ( CButton* )GetDlgItem( IDC_RADIO_MODE_HORIZONTAL ) )->EnableWindow( FALSE );
	( ( CButton* )GetDlgItem( IDC_RADIO_MODE_VERTICAL   ) )->EnableWindow( FALSE );

	( ( CButton* )GetDlgItem( IDC_RADIO_MODE_HORIZONTAL ) )->SetCheck( TRUE );

	HANDLE hdFile;  
	WIN32_FIND_DATA fdFileData;
	m_caCfgFileList.RemoveAll();
	hdFile = FindFirstFile( m_csCfgFileName.GetBuffer(), &fdFileData );  
	if ( (Uint32)hdFile != 0xFFFFFFFF )
	{
		m_caCfgFileList.Add( fdFileData );
		while( FindNextFile( hdFile, &fdFileData) )		m_caCfgFileList.Add( fdFileData );  
	}
	else
	{
		m_btCfgLoad.EnableWindow( FALSE );
	}
	
	Uint32 u32FileNum = m_caCfgFileList.GetCount();
	for ( Uint32 u32Index = 0; u32Index < u32FileNum; u32Index++ )
	{
		m_cmbCfgFileName.AddString( m_caCfgFileList.GetAt( u32Index ).cFileName );
	}
	m_cmbCfgFileName.SetCurSel( 0 );
	//SetTimer( DISP_INF_TIMER, 1000, NULL );
	SetTimer( SCAN_TIMER,     2000, NULL );

	m_u32CaptFlag = 0;
	m_u32ReadFlag = 0;
	
	m_s32LocXMoved = 0xFFFF;
	m_s32LocYMoved = 0xFFFF;

	m_s32locStartX = 0;
	m_s32locStartY = 0;

	m_pixScale = 1;
	m_s32ScaleIndex = 10;

	DlgFrameSet();

	FillBlackDisplay();

	m_u8FullScreenEn	  = 1;
	m_u8WhiteBalanceEn	  = 0;
	//m_u8WhiteBalanceCfgEn = 0;
	m_reInf.SetEventMask(ENM_MOUSEEVENTS);
	return TRUE;  
}
 

void CUSBTestDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ÓÃÓÚ»æÖÆµÄÉè±¸ÉÏÏÂÎÄ

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Ê¹Í¼±êÔÚ¹¤×÷¾ØÐÎÖÐ¾ÓÖÐ
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// »æÖÆÍ¼±ê
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}
  

HCURSOR CUSBTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}
  

void CUSBTestDlg::DlgFrameSet( void )
{
	RECT m_btScan_rect;
	m_cmbUsbIndex.GetWindowRect(&m_btScan_rect);
	ScreenToClient(&m_btScan_rect);

	RECT m_rcLogDisp_rect;
	m_chkForceDisp.GetWindowRect(&m_rcLogDisp_rect);
	ScreenToClient(&m_rcLogDisp_rect);

	m_u32DisplayHeight = m_rcLogDisp_rect.bottom - m_btScan_rect.top;
	m_u32DisplayWidth  = m_u32DisplayHeight * 4 / 3;

	m_sttDisplay_rect.left   = m_btScan_rect.right + FRAME_W_GAP;
	m_sttDisplay_rect.right  = m_btScan_rect.right + FRAME_W_GAP + m_u32DisplayWidth;
	m_sttDisplay_rect.top    = m_btScan_rect.top;
	m_sttDisplay_rect.bottom = m_btScan_rect.top + m_u32DisplayHeight;

	m_sttDisplay.SetWindowPos( NULL, m_sttDisplay_rect.left, m_sttDisplay_rect.top, m_u32DisplayWidth, m_u32DisplayHeight, 0 );

	RECT m_dlg_rect;
	GetWindowRect(&m_dlg_rect);
	SetWindowPos( NULL, m_dlg_rect.left, m_dlg_rect.top, m_sttDisplay_rect.right + 15, m_dlg_rect.bottom - m_dlg_rect.top, 0 );

	RECT m_rcFmtMode_rect;
	m_btRawMode.GetWindowRect(&m_rcFmtMode_rect);
	ScreenToClient(&m_rcFmtMode_rect);

	m_btRawMode.SetWindowPos( NULL, m_sttDisplay_rect.right - ( m_rcFmtMode_rect.right - m_rcFmtMode_rect.left ),
									m_rcFmtMode_rect.top, 
									m_rcFmtMode_rect.right  - m_rcFmtMode_rect.left, 
									m_rcFmtMode_rect.bottom - m_rcFmtMode_rect.top, 
									0 );
}
  

void CUSBTestDlg::OnTimer(UINT_PTR nIDEvent)
{
	switch (nIDEvent)
	{
		/* --------------------------------------------------------------------------------------------------- */
		/* ----- ×Ô¼º±àÐ´µÄ²¿·Ö																		 ----- */
		/* --------------------------------------------------------------------------------------------------- */
		case	DISP_INF_TIMER:				DispInfo();					break;
		//case	SCAN_TIMER:					OnBnClickedButtonScan();	break;
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
		/* É¾³ýÒ»°ë³¤¶È */
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
		case USB_CAMERA_DATA_LEN_ERROR:			csErrorCode.Format( "Bad frame received" );					break;
		case USB_CAMERA_FRAME_INDEX_ERROR:		csErrorCode.Format( "USB transfer data index error" );		break;
		case USB_CAMERA_USB_TIMEOUT_ERROR:		csErrorCode.Format( "USB transfer timeout error" );			break;

		case USB_CAMERA_READ_EMPTY_ERROR :		csErrorCode.Format( "camera frame read empty error" );		break;
		case USB_CAMERA_DEL_EMPTY_ERROR :		csErrorCode.Format( "camera frame delete empty error" );	break;

		case USB_CAMERA_SIZE_EXCEED_ERROR:		csErrorCode.Format( "camera size exceed error" );			break;

		default :								csErrorCode.Format( "unknown error" );						break;
	}

	if ( u32ErrorCode != USB_CAMERA_NO_ERROR )		InsertText( csErrorCode, OUTPUT_RED );

}
  
int CUSBTestDlg::filetime_WIN32(int64 *filetime_int64)
{

#ifdef _WIN32
	FILETIME fileTime;
	ULARGE_INTEGER currentFileTime;
	GetSystemTimeAsFileTime(&fileTime);
	currentFileTime.HighPart = fileTime.dwHighDateTime;
	currentFileTime.LowPart = fileTime.dwLowDateTime;
	*filetime_int64 = (int64)currentFileTime.QuadPart;
	return 0;
#else
	*filetime_int64 = -1;
	return -1;
#endif
}

void CUSBTestDlg::DispInfo( void )
{
	CString csTmpString;

	csTmpString.Format( "Capt: %d fps", m_u32FrameCaptureCnt - m_u32FrameCaptureCntBak );
	m_sttFrameCaptureCnt.SetWindowText( csTmpString );

	csTmpString.Format( "Read: %d fps", m_u32FrameReadCnt - m_u32FrameReadCntBak );
	m_sttFrameReadCnt.SetWindowText( csTmpString );

	m_u32FrameCaptureCntBak = m_u32FrameCaptureCnt;
	m_u32FrameReadCntBak    = m_u32FrameReadCnt;

	ArduCam_getboardConfig( m_cUsbCameraHd, 0xB0, 0, 0, sizeof(UTC_PARA_OUT), (Uint8*) &stpstUtcParaOut );
	TRACE("A = %8f, B = %8f\n", stpstUtcParaOut.f64A, stpstUtcParaOut.f64B);
	stUtcParaIn.s64C0 = stpstUtcParaOut.s64C0;

	//stUtcParaIn.s64U0 = clock();
	filetime_WIN32(&stUtcParaIn.s64U0);
	TRACE("U0 = %I64u\n", stUtcParaIn.s64U0);
	ArduCam_setboardConfig( m_cUsbCameraHd, 0xB1, 0, 0, sizeof(UTC_PARA_IN), (Uint8*) &stUtcParaIn );
	TRACE("C0 = %d, U0 = %I64u\n",  stUtcParaIn.s64C0, stUtcParaIn.s64U0);

}
    
   
void CUSBTestDlg::FrameDisplayBMP( Uint8* pu8RgbData, Uint32 u32Width, Uint32 u32Height )
{
	CDC		*pcdDC;
	CRect	crRect;	
	CWnd	*pcwWnd = GetDlgItem( IDC_STATIC_DISPLAY );
	pcwWnd->GetClientRect( &crRect );
	pcdDC = pcwWnd->GetDC();

	ArduCamDisp_FrameDisplayBMP( pcdDC, &crRect, pu8RgbData, u32Width, u32Height, 
								 m_u32DisplayWidth, m_u32DisplayHeight, m_s32locStartX, m_s32locStartY, 
								 m_pixScale, m_u8FullScreenEn );

	ReleaseDC( pcdDC );
}


void CUSBTestDlg::FrameDisplayBMPMirror(Uint8* pu8RgbData, Uint32 u32Width, Uint32 u32Height)
{
	CDC		*pcdDC;
	CRect	crRect;
	CWnd	*pcwWnd = GetDlgItem(IDC_STATIC_DISPLAY);
	pcwWnd->GetClientRect(&crRect);
	pcdDC = pcwWnd->GetDC();

	ArduCamDisp_FrameDisplayBMPMirror( pcdDC, &crRect, pu8RgbData, u32Width, u32Height,
									   m_u32DisplayWidth, m_u32DisplayHeight, m_s32locStartX, m_s32locStartY,
									   m_pixScale, m_u8FullScreenEn);

	ReleaseDC(pcdDC);
}

void CUSBTestDlg::FrameDisplayJPG( Uint8* pu8JpgData, Uint32 u32Size )
{
	CDC	*pcdDC;
	pcdDC = GetDC();

	CRect crRect;	
	GetDlgItem( IDC_STATIC_DISPLAY )->GetWindowRect(&crRect);
	ScreenToClient(&crRect);

	ArduCamDisp_FrameDisplayJPG( pcdDC, &crRect, pu8JpgData, u32Size,
								 m_u32DisplayWidth, m_u32DisplayHeight, m_s32locStartX, m_s32locStartY, 
								 m_pixScale, m_u8FullScreenEn );

	ReleaseDC( pcdDC );
}
  
   
void CUSBTestDlg::CaptureFrame( void )
{
	Uint32 u32TmpState;

	u32TmpState = ArduCam_beginCaptureImage( m_cUsbCameraHd );
	if ( USB_CAMERA_USB_TASK_ERROR == u32TmpState )
	{
		OnBnClickedButtonStop();
		m_u32CaptFlag = 0;
		OnBnClickedButtonClose();
		DispErrorLog( u32TmpState );
		return;
	}
	
	while ( m_u32FrameCaptureThreadEn == THREAD_ENABLE )
	{
		u32TmpState = ArduCam_captureImage( m_cUsbCameraHd );
		if ( u32TmpState == USB_CAMERA_USB_TASK_ERROR )
		{
			OnBnClickedButtonStop();
			m_u32CaptFlag = 0;
			OnBnClickedButtonClose();
			DispErrorLog( u32TmpState );
			return;
		}

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

	ArduCamOutData*	pstTmpFrameData = NULL;

	while ( m_u32FrameReadThreadEn == THREAD_ENABLE )
	{
		if ( ArduCam_availableImage( m_cUsbCameraHd ) > 0 )
		{
			u32TmpState = ArduCam_readImage( m_cUsbCameraHd, pstTmpFrameData );
			
			if ( u32TmpState == USB_CAMERA_NO_ERROR )
			{
				//TRACE( "TIME: %04d-%02d-%02d %02d:%02d:%02d %03d\r\n",    pstTmpFrameData->stTime.wYear,
				//														  pstTmpFrameData->stTime.wMonth,
				//														  pstTmpFrameData->stTime.wDay,
				//														  pstTmpFrameData->stTime.wHour,
				//														  pstTmpFrameData->stTime.wMinute,
				//														  pstTmpFrameData->stTime.wSecond,
				//														  pstTmpFrameData->stTime.wMilliseconds );
				TRACE( "UTC Time: %I64u \r\n", pstTmpFrameData->u64Time );

				m_u32FrameReadCnt++;

				CString csTmpString;

				//if ( m_u8WhiteBalanceEn == 1 )
				//{
				//	Int8 s8GainOffset[3];

				//	if ( pstTmpFrameData->stImagePara.emImageFmtMode == FORMAT_MODE_RAW )
				//	{
				//		ArduCamDisp_RawWhiteBalancePara( pstTmpFrameData->pu8ImageData, pstTmpFrameData->stImagePara.u32Width, pstTmpFrameData->stImagePara.u32Height, m_u32RawMode, pstTmpFrameData->stImagePara.u8PixelBits, s8GainOffset );
				//		SetWhiteBalanceCfg( s8GainOffset );
				//	}
				//}

				if ( m_u8RecordEn == RECORD_NULL )
				{
					switch ( pstTmpFrameData->stImagePara.emImageFmtMode )
					{
					case FORMAT_MODE_RAW:
						ArduCamDisp_Raw2rgb24( m_pu8OutRgb24, pstTmpFrameData->pu8ImageData, pstTmpFrameData->stImagePara.u32Width, pstTmpFrameData->stImagePara.u32Height, m_u32RawMode, pstTmpFrameData->stImagePara.u8PixelBits );
						
						if (m_u8WhiteBalanceEn == 1) {
							ArduCamDisp_RgbWhiteBalance(m_pu8OutRgb24, m_pu8OutRgb24, pstTmpFrameData->stImagePara.u32Width, pstTmpFrameData->stImagePara.u32Height);
						}

						FrameDisplayBMP( m_pu8OutRgb24, pstTmpFrameData->stImagePara.u32Width, pstTmpFrameData->stImagePara.u32Height );
						break;
					case FORMAT_MODE_MON:
						ArduCamDisp_Mono2rgb24( m_pu8OutRgb24, pstTmpFrameData->pu8ImageData, pstTmpFrameData->stImagePara.u32Width, pstTmpFrameData->stImagePara.u32Height, pstTmpFrameData->stImagePara.u8PixelBits );
						FrameDisplayBMP( m_pu8OutRgb24, pstTmpFrameData->stImagePara.u32Width, pstTmpFrameData->stImagePara.u32Height );
						break;
					case FORMAT_MODE_RGB: 
						ArduCamDisp_Rgb5652rgb24( m_pu8OutRgb24, pstTmpFrameData->pu8ImageData, pstTmpFrameData->stImagePara.u32Width, pstTmpFrameData->stImagePara.u32Height, m_u32RgbMode );
						FrameDisplayBMP( m_pu8OutRgb24, pstTmpFrameData->stImagePara.u32Width, pstTmpFrameData->stImagePara.u32Height );
						break;
					case FORMAT_MODE_YUV:
						ArduCamDisp_Yuv4222rgb24( m_pu8OutRgb24, pstTmpFrameData->pu8ImageData, pstTmpFrameData->stImagePara.u32Width, pstTmpFrameData->stImagePara.u32Height, m_u32YuvMode );
						FrameDisplayBMP( m_pu8OutRgb24, pstTmpFrameData->stImagePara.u32Width, pstTmpFrameData->stImagePara.u32Height );
						break;
					case FORMAT_MODE_JPG:
						m_u32JpgSize = pstTmpFrameData->stImagePara.u32Size;
						memcpy( m_pu8OutRgb24, pstTmpFrameData->pu8ImageData, pstTmpFrameData->stImagePara.u32Size * sizeof( Uint8 ) );
						FrameDisplayJPG( m_pu8OutRgb24, m_u32JpgSize );
						break;
					case FORMAT_MODE_RAW_D:
						ArduCamDisp_RawD2rgb24( m_pu8OutRgb24, pstTmpFrameData->pu8ImageData, pstTmpFrameData->stImagePara.u32Width, pstTmpFrameData->stImagePara.u32Height, m_u32RawMode, pstTmpFrameData->stImagePara.u8PixelBits, m_u8DImageDispMode );
						if ( m_u8DImageDispMode == MODE_HORIZONTAL ) FrameDisplayBMP( m_pu8OutRgb24, pstTmpFrameData->stImagePara.u32Width*2, pstTmpFrameData->stImagePara.u32Height   );
						if ( m_u8DImageDispMode == MODE_VERTICAL )   FrameDisplayBMP( m_pu8OutRgb24, pstTmpFrameData->stImagePara.u32Width  , pstTmpFrameData->stImagePara.u32Height*2 );
						break;
					case FORMAT_MODE_MON_D:
						ArduCamDisp_MonoD2rgb24( m_pu8OutRgb24, pstTmpFrameData->pu8ImageData, pstTmpFrameData->stImagePara.u32Width, pstTmpFrameData->stImagePara.u32Height, pstTmpFrameData->stImagePara.u8PixelBits, m_u8DImageDispMode );
						if ( m_u8DImageDispMode == MODE_HORIZONTAL ) FrameDisplayBMP( m_pu8OutRgb24, pstTmpFrameData->stImagePara.u32Width*2, pstTmpFrameData->stImagePara.u32Height   );
						if ( m_u8DImageDispMode == MODE_VERTICAL )   FrameDisplayBMP( m_pu8OutRgb24, pstTmpFrameData->stImagePara.u32Width,   pstTmpFrameData->stImagePara.u32Height*2 );
						break;
					case FORMAT_MODE_TOF:
						ArduCamDisp_Tof2rgb24(m_pu8OutRgb24, pstTmpFrameData->pu8ImageData, pstTmpFrameData->stImagePara.u32Width, pstTmpFrameData->stImagePara.u32Height, pstTmpFrameData->stImagePara.u8PixelBits);
						FrameDisplayBMP(m_pu8OutRgb24, pstTmpFrameData->stImagePara.u32Width * 2, pstTmpFrameData->stImagePara.u32Height * 2);
						break;
					}
				}
				else if ( m_u8RecordEn == RECORD_IMAGE )
				{
					csTmpString = CreateImageFileName( &( pstTmpFrameData->stImagePara ) ); 
					
					switch ( pstTmpFrameData->stImagePara.emImageFmtMode )
					{
					case FORMAT_MODE_RAW:
						ArduCamDisp_Raw2rgb24( m_pu8OutRgb24, pstTmpFrameData->pu8ImageData, pstTmpFrameData->stImagePara.u32Width, pstTmpFrameData->stImagePara.u32Height, m_u32RawMode, pstTmpFrameData->stImagePara.u8PixelBits );
						ArduCamDisp_RecordImageBMP( csTmpString, m_pu8OutRgb24, pstTmpFrameData->stImagePara.u32Width, pstTmpFrameData->stImagePara.u32Height );
						break;
					case FORMAT_MODE_MON:
						ArduCamDisp_Mono2rgb24( m_pu8OutRgb24, pstTmpFrameData->pu8ImageData, pstTmpFrameData->stImagePara.u32Width, pstTmpFrameData->stImagePara.u32Height, pstTmpFrameData->stImagePara.u8PixelBits );
						ArduCamDisp_RecordImageBMP( csTmpString, m_pu8OutRgb24, pstTmpFrameData->stImagePara.u32Width, pstTmpFrameData->stImagePara.u32Height );
						break;
					case FORMAT_MODE_RGB: 
						ArduCamDisp_Rgb5652rgb24( m_pu8OutRgb24, pstTmpFrameData->pu8ImageData, pstTmpFrameData->stImagePara.u32Width, pstTmpFrameData->stImagePara.u32Height, m_u32RgbMode );
						ArduCamDisp_RecordImageBMP( csTmpString, m_pu8OutRgb24, pstTmpFrameData->stImagePara.u32Width, pstTmpFrameData->stImagePara.u32Height );
						break;
					case FORMAT_MODE_YUV:
						ArduCamDisp_Yuv4222rgb24( m_pu8OutRgb24, pstTmpFrameData->pu8ImageData, pstTmpFrameData->stImagePara.u32Width, pstTmpFrameData->stImagePara.u32Height, m_u32YuvMode );
						ArduCamDisp_RecordImageBMP( csTmpString, m_pu8OutRgb24, pstTmpFrameData->stImagePara.u32Width, pstTmpFrameData->stImagePara.u32Height );
						break;
					case FORMAT_MODE_JPG:
						m_u32JpgSize = pstTmpFrameData->stImagePara.u32Size;
						memcpy( m_pu8OutRgb24, pstTmpFrameData->pu8ImageData, pstTmpFrameData->stImagePara.u32Size * sizeof( Uint8 ) );
						ArduCamDisp_RecordImageJPG( csTmpString, m_pu8OutRgb24, m_u32JpgSize );
						break;
					case FORMAT_MODE_RAW_D:
						ArduCamDisp_RawD2rgb24( m_pu8OutRgb24, pstTmpFrameData->pu8ImageData, pstTmpFrameData->stImagePara.u32Width, pstTmpFrameData->stImagePara.u32Height, m_u32RawMode, pstTmpFrameData->stImagePara.u8PixelBits, m_u8DImageDispMode );
						if ( m_u8DImageDispMode == MODE_HORIZONTAL ) ArduCamDisp_RecordImageBMP( csTmpString, m_pu8OutRgb24, pstTmpFrameData->stImagePara.u32Width*2, pstTmpFrameData->stImagePara.u32Height   );
						if ( m_u8DImageDispMode == MODE_VERTICAL )   ArduCamDisp_RecordImageBMP( csTmpString, m_pu8OutRgb24, pstTmpFrameData->stImagePara.u32Width,   pstTmpFrameData->stImagePara.u32Height*2 );
						break;
					case FORMAT_MODE_MON_D:
						ArduCamDisp_MonoD2rgb24( m_pu8OutRgb24, pstTmpFrameData->pu8ImageData, pstTmpFrameData->stImagePara.u32Width, pstTmpFrameData->stImagePara.u32Height, pstTmpFrameData->stImagePara.u8PixelBits, m_u8DImageDispMode );
						if ( m_u8DImageDispMode == MODE_HORIZONTAL ) ArduCamDisp_RecordImageBMP( csTmpString, m_pu8OutRgb24, pstTmpFrameData->stImagePara.u32Width*2, pstTmpFrameData->stImagePara.u32Height   );
						if ( m_u8DImageDispMode == MODE_VERTICAL )   ArduCamDisp_RecordImageBMP( csTmpString, m_pu8OutRgb24, pstTmpFrameData->stImagePara.u32Width,   pstTmpFrameData->stImagePara.u32Height*2 );
						break;
					case FORMAT_MODE_TOF:
						ArduCamDisp_Tof2rgb24(m_pu8OutRgb24, pstTmpFrameData->pu8ImageData, pstTmpFrameData->stImagePara.u32Width, pstTmpFrameData->stImagePara.u32Height, pstTmpFrameData->stImagePara.u8PixelBits);
						ArduCamDisp_RecordImageBMP(csTmpString, m_pu8OutRgb24, pstTmpFrameData->stImagePara.u32Width * 2, pstTmpFrameData->stImagePara.u32Height * 2);
						break;
					}
				}
				else if ( m_u8RecordEn == RECORD_DATA )
				{
					csTmpString = CreateDataFileName( &( pstTmpFrameData->stImagePara ) ); 
					ArduCamDisp_RecordImageData( csTmpString, pstTmpFrameData->pu8ImageData, pstTmpFrameData->stImagePara.u32Size );
				}
				else if ( m_u8RecordEn == RECORD_VIDEO )
				{
					ArduCamCfg stTmpCfg = pstTmpFrameData->stImagePara;
					
					switch ( pstTmpFrameData->stImagePara.emImageFmtMode )
					{
					case FORMAT_MODE_RAW:
						ArduCamDisp_Raw2rgb24( m_pu8OutRgb24, pstTmpFrameData->pu8ImageData, pstTmpFrameData->stImagePara.u32Width, pstTmpFrameData->stImagePara.u32Height, m_u32RawMode, pstTmpFrameData->stImagePara.u8PixelBits );
						break;
					case FORMAT_MODE_MON:
						ArduCamDisp_Mono2rgb24( m_pu8OutRgb24, pstTmpFrameData->pu8ImageData, pstTmpFrameData->stImagePara.u32Width, pstTmpFrameData->stImagePara.u32Height, pstTmpFrameData->stImagePara.u8PixelBits );
						break;
					case FORMAT_MODE_RGB: 
						ArduCamDisp_Rgb5652rgb24( m_pu8OutRgb24, pstTmpFrameData->pu8ImageData, pstTmpFrameData->stImagePara.u32Width, pstTmpFrameData->stImagePara.u32Height, m_u32RgbMode );
						break;
					case FORMAT_MODE_YUV:
						ArduCamDisp_Yuv4222rgb24( m_pu8OutRgb24, pstTmpFrameData->pu8ImageData, pstTmpFrameData->stImagePara.u32Width, pstTmpFrameData->stImagePara.u32Height, m_u32YuvMode );
						break;
					case FORMAT_MODE_JPG:
						m_u32JpgSize = pstTmpFrameData->stImagePara.u32Size;
						memcpy( m_pu8OutRgb24, pstTmpFrameData->pu8ImageData, pstTmpFrameData->stImagePara.u32Size * sizeof( Uint8 ) );
						break;
					case FORMAT_MODE_RAW_D:
						ArduCamDisp_RawD2rgb24( m_pu8OutRgb24, pstTmpFrameData->pu8ImageData, pstTmpFrameData->stImagePara.u32Width, pstTmpFrameData->stImagePara.u32Height, m_u32RawMode, pstTmpFrameData->stImagePara.u8PixelBits, m_u8DImageDispMode );
						if ( m_u8DImageDispMode == MODE_HORIZONTAL ) stTmpCfg.u32Width  = stTmpCfg.u32Width  * 2;
						if ( m_u8DImageDispMode == MODE_VERTICAL )   stTmpCfg.u32Height = stTmpCfg.u32Height * 2;
						break;
					case FORMAT_MODE_MON_D:
						ArduCamDisp_MonoD2rgb24( m_pu8OutRgb24, pstTmpFrameData->pu8ImageData, pstTmpFrameData->stImagePara.u32Width, pstTmpFrameData->stImagePara.u32Height, pstTmpFrameData->stImagePara.u8PixelBits, m_u8DImageDispMode );
						if ( m_u8DImageDispMode == MODE_HORIZONTAL ) stTmpCfg.u32Width  = stTmpCfg.u32Width  * 2;
						if ( m_u8DImageDispMode == MODE_VERTICAL )   stTmpCfg.u32Height = stTmpCfg.u32Height * 2;
						break;
					case FORMAT_MODE_TOF:
						ArduCamDisp_Tof2rgb24(m_pu8OutRgb24, pstTmpFrameData->pu8ImageData, pstTmpFrameData->stImagePara.u32Width, pstTmpFrameData->stImagePara.u32Height, pstTmpFrameData->stImagePara.u8PixelBits);
						stTmpCfg.u32Width *= 2;
						stTmpCfg.u32Height *= 2;
						break;
					}
					ArduCamDisp_RecordVideo( m_cUsbCameraRecordHd, m_pu8OutRgb24, &stTmpCfg );

					if ( m_u8RecordEnd != RECORD_VIDEO )
					{
						ArduCamDisp_EndRecordVideo( m_cUsbCameraRecordHd );
						m_u8RecordEn = m_u8RecordEnd;
					}
					
				}

				if ( m_u32ShotTimeFlag == 1 )
				{
					m_u32ShotTimeFlag = 0;

					SYSTEMTIME tiTmpTime;
					GetLocalTime( &tiTmpTime );
					CString csTmpValue, csTmpString;

					csTmpValue.Format( "\\%04d%02d%02d_%02d%02d%02d_Image.", tiTmpTime.wYear, tiTmpTime.wMonth, tiTmpTime.wDay,
																			 tiTmpTime.wHour, tiTmpTime.wMinute, tiTmpTime.wSecond );
					if ( pstTmpFrameData->stImagePara.emImageFmtMode == FORMAT_MODE_JPG )
					{
						csTmpString = m_csShotPath + csTmpValue + "jpg";
						ArduCamDisp_RecordImageJPG( csTmpString, m_pu8OutRgb24, m_u32JpgSize );
					}
					else if (    ( pstTmpFrameData->stImagePara.emImageFmtMode == FORMAT_MODE_RAW_D )
						      || ( pstTmpFrameData->stImagePara.emImageFmtMode == FORMAT_MODE_MON_D ) )
					{
						csTmpString = m_csShotPath + csTmpValue + "bmp";
						if ( m_u8DImageDispMode == MODE_HORIZONTAL ) ArduCamDisp_RecordImageBMP( csTmpString, m_pu8OutRgb24, pstTmpFrameData->stImagePara.u32Width*2, pstTmpFrameData->stImagePara.u32Height   );
						if ( m_u8DImageDispMode == MODE_VERTICAL )   ArduCamDisp_RecordImageBMP( csTmpString, m_pu8OutRgb24, pstTmpFrameData->stImagePara.u32Width,   pstTmpFrameData->stImagePara.u32Height*2 );
					}
					else
					{
						csTmpString = m_csShotPath + csTmpValue + "bmp";
						ArduCamDisp_RecordImageBMP( csTmpString, m_pu8OutRgb24, pstTmpFrameData->stImagePara.u32Width, pstTmpFrameData->stImagePara.u32Height );
					}
				}

				ArduCam_del( m_cUsbCameraHd );
			} 
			else
			{
				DispErrorLog( u32TmpState );
			}			
		}
		else
		{
//			Sleep( 1 );
		}
	}

	m_u32FrameReadThreadEn = THREAD_DISABLE;

	m_u32ReadFlag = 0;
				
	return;
}


void CUSBTestDlg::OnBnClickedButtonScan()
{
	CString	csTmpString;

	ArduCamIndexinfo stUsbIndexArray[100];

	Uint32 u32UsbNum = ArduCam_scan( stUsbIndexArray );

	m_cmbUsbIndex.ResetContent();
	for ( Uint32 u32Index = 0; u32Index < u32UsbNum; u32Index++ )
	{
		csTmpString.Format( "%d  %c%c%c%c-%c%c%c%c-%c%c%c%c", stUsbIndexArray[u32Index].u8UsbIndex,
															stUsbIndexArray[u32Index].u8SerialNum[0],
															stUsbIndexArray[u32Index].u8SerialNum[1],
															stUsbIndexArray[u32Index].u8SerialNum[2],
															stUsbIndexArray[u32Index].u8SerialNum[3],
															stUsbIndexArray[u32Index].u8SerialNum[4],
															stUsbIndexArray[u32Index].u8SerialNum[5],
															stUsbIndexArray[u32Index].u8SerialNum[6],
															stUsbIndexArray[u32Index].u8SerialNum[7],
															stUsbIndexArray[u32Index].u8SerialNum[8],
															stUsbIndexArray[u32Index].u8SerialNum[9],
															stUsbIndexArray[u32Index].u8SerialNum[10],
															stUsbIndexArray[u32Index].u8SerialNum[11] );

		m_cmbUsbIndex.AddString( csTmpString );
	}

	if ( u32UsbNum > 0 )		m_cmbUsbIndex.SetCurSel( 0 );
}
  

void CUSBTestDlg::OnBnClickedButtonOpen()
{
	CString	csTmpString;

	Int32 s32Index = m_cmbUsbIndex.GetCurSel();
	Uint32 u32TmpUsbIndex = 0;
	if ( s32Index >= 0 )
	{
		m_cmbUsbIndex.GetLBText( s32Index, csTmpString );
		u32TmpUsbIndex = atoi( csTmpString );
	}

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

	switch ( m_cmbImgFormat.GetCurSel() )
	{
		case 0:		stTmpUsbCameraCfg.emImageFmtMode = FORMAT_MODE_RAW;		break;
		case 1:		stTmpUsbCameraCfg.emImageFmtMode = FORMAT_MODE_RGB;		break;
		case 2:		stTmpUsbCameraCfg.emImageFmtMode = FORMAT_MODE_YUV;		break;
		case 3:		stTmpUsbCameraCfg.emImageFmtMode = FORMAT_MODE_JPG;		break;
		case 4:		stTmpUsbCameraCfg.emImageFmtMode = FORMAT_MODE_MON;		break;
		case 5:		stTmpUsbCameraCfg.emImageFmtMode = FORMAT_MODE_RAW_D;	break;
		case 6:		stTmpUsbCameraCfg.emImageFmtMode = FORMAT_MODE_MON_D;	break;
		case 7:		stTmpUsbCameraCfg.emImageFmtMode = FORMAT_MODE_TOF;		break;
		default:	stTmpUsbCameraCfg.emImageFmtMode = FORMAT_MODE_RAW;		break;
	}

	switch ( m_cmbI2cMode.GetCurSel() )
	{
		case 0:		stTmpUsbCameraCfg.emI2cMode = I2C_MODE_8_8;			break;
		case 1:		stTmpUsbCameraCfg.emI2cMode = I2C_MODE_8_16;		break;
		case 2:		stTmpUsbCameraCfg.emI2cMode = I2C_MODE_16_8;		break;
		case 3:		stTmpUsbCameraCfg.emI2cMode = I2C_MODE_16_16;		break;
		default:	stTmpUsbCameraCfg.emI2cMode = I2C_MODE_8_16;		break;
	}

	m_edtI2cAddr.GetWindowText( csTmpString );
	stTmpUsbCameraCfg.u32I2cAddr = atoi_ex( csTmpString );

	m_edtVid.GetWindowText( csTmpString );
	stTmpUsbCameraCfg.u16Vid  = atoi_ex( csTmpString );

	stTmpUsbCameraCfg.u32TransLvl = m_u32TransLvl;

	Uint32 u32Value = 0;	
	if ( s32Index >= 0 )	u32Value = ArduCam_open( m_cUsbCameraHd, &stTmpUsbCameraCfg, u32TmpUsbIndex );
	else					u32Value = ArduCam_autoopen( m_cUsbCameraHd, &stTmpUsbCameraCfg );

	DispErrorLog( u32Value );

	if ( u32Value == USB_CAMERA_NO_ERROR )
	{
		//if ( CFG_BOARD_ERROR == ArduCamCfg_SetBoardConfig( m_cUsbCameraCfgHd, m_cUsbCameraHd ) )
		//	InsertText( "Board config error!", OUTPUT_RED );
		
		if ( CFG_REGISTER_ERROR == ArduCamCfg_SetRegisterConfig( m_cUsbCameraCfgHd, m_cUsbCameraHd ) )
			InsertText( "Register config error!", OUTPUT_RED );
		
		m_btPlay.EnableWindow( TRUE );
		m_btStop.EnableWindow( FALSE );
		m_btShot.EnableWindow(FALSE);

		m_btInit.EnableWindow( FALSE );
		m_btOpen.EnableWindow( FALSE );
		m_btClose.EnableWindow( TRUE );

		m_btRegRead.EnableWindow( TRUE );
		m_btRegWrite.EnableWindow( TRUE );

		( ( CButton* )GetDlgItem( IDC_RADIO_DISPLAY    ) )->EnableWindow( TRUE );
		( ( CButton* )GetDlgItem( IDC_RADIO_SAVE_DATA  ) )->EnableWindow( TRUE );
		( ( CButton* )GetDlgItem( IDC_RADIO_SAVE_IMAGE ) )->EnableWindow( TRUE );
		( ( CButton* )GetDlgItem( IDC_RADIO_SAVE_VIDEO ) )->EnableWindow( TRUE );

		m_chkIRcut.EnableWindow( TRUE );
		m_chkIRcut.SetCheck( FALSE );

		//if ( m_u8WhiteBalanceCfgEn == 0x7 )		m_chkWhiteBalance.EnableWindow( TRUE );
		if(stTmpUsbCameraCfg.emImageFmtMode == FORMAT_MODE_RAW )	m_chkWhiteBalance.EnableWindow(TRUE);

		if ( stTmpUsbCameraCfg.emImageFmtMode != FORMAT_MODE_JPG )
		{
			m_chkForceDisp.EnableWindow( TRUE );
			m_chkForceDisp.SetCheck( FALSE );
		}

		if (    ( stTmpUsbCameraCfg.emImageFmtMode == FORMAT_MODE_RAW_D ) 
			 || ( stTmpUsbCameraCfg.emImageFmtMode == FORMAT_MODE_MON_D ) )
		{
			( ( CButton* )GetDlgItem( IDC_RADIO_MODE_HORIZONTAL ) )->EnableWindow( TRUE );
			( ( CButton* )GetDlgItem( IDC_RADIO_MODE_VERTICAL   ) )->EnableWindow( TRUE );
		}

		m_u32FrameReadThreadEn = THREAD_ENABLE;												
		m_ptdFrameReadThread   = AfxBeginThread( _FrameReadThread, this );

		Uint8 u8TmpData[16];
		ArduCam_readUserData( m_cUsbCameraHd, 0x400-16, 16, u8TmpData );

		csTmpString.Format( "Serial: %c%c%c%c-%c%c%c%c-%c%c%c%c",	
							u8TmpData[0], u8TmpData[1], u8TmpData[2], u8TmpData[3], 
							u8TmpData[4], u8TmpData[5], u8TmpData[6], u8TmpData[7], 
							u8TmpData[8], u8TmpData[9], u8TmpData[10], u8TmpData[11] );

		m_sttSerial.SetWindowText( csTmpString );

		InsertText( "USB camera init success!", OUTPUT_BLUE );
		SetTimer(DISP_INF_TIMER, 1000, NULL);
	}
	
}
  

void CUSBTestDlg::OnBnClickedButtonInit()
{
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

	switch ( m_cmbImgFormat.GetCurSel() )
	{
		case 0:		stTmpUsbCameraCfg.emImageFmtMode = FORMAT_MODE_RAW;		break;
		case 1:		stTmpUsbCameraCfg.emImageFmtMode = FORMAT_MODE_RGB;		break;
		case 2:		stTmpUsbCameraCfg.emImageFmtMode = FORMAT_MODE_YUV;		break;
		case 3:		stTmpUsbCameraCfg.emImageFmtMode = FORMAT_MODE_JPG;		break;
		case 4:		stTmpUsbCameraCfg.emImageFmtMode = FORMAT_MODE_MON;		break;
		case 5:		stTmpUsbCameraCfg.emImageFmtMode = FORMAT_MODE_RAW_D;	break;
		case 6:		stTmpUsbCameraCfg.emImageFmtMode = FORMAT_MODE_MON_D;	break;
		case 7:		stTmpUsbCameraCfg.emImageFmtMode = FORMAT_MODE_TOF;		break;
		default:	stTmpUsbCameraCfg.emImageFmtMode = FORMAT_MODE_RAW;		break;
	}

	switch ( m_cmbI2cMode.GetCurSel() )
	{
		case 0:		stTmpUsbCameraCfg.emI2cMode = I2C_MODE_8_8;			break;
		case 1:		stTmpUsbCameraCfg.emI2cMode = I2C_MODE_8_16;		break;
		case 2:		stTmpUsbCameraCfg.emI2cMode = I2C_MODE_16_8;		break;
		case 3:		stTmpUsbCameraCfg.emI2cMode = I2C_MODE_16_16;		break;
		default:	stTmpUsbCameraCfg.emI2cMode = I2C_MODE_8_16;		break;
	}

	m_edtI2cAddr.GetWindowText( csTmpString );
	stTmpUsbCameraCfg.u32I2cAddr = atoi_ex( csTmpString );

	m_edtVid.GetWindowText( csTmpString );
	stTmpUsbCameraCfg.u16Vid  = atoi_ex( csTmpString );

	stTmpUsbCameraCfg.u32TransLvl = m_u32TransLvl;

	Uint32 u32Value = ArduCam_autoopen( m_cUsbCameraHd, &stTmpUsbCameraCfg );

	DispErrorLog( u32Value );

	if ( u32Value == USB_CAMERA_NO_ERROR )
	{
		//if ( CFG_BOARD_ERROR == ArduCamCfg_SetBoardConfig( m_cUsbCameraCfgHd, m_cUsbCameraHd ) )
		//	InsertText( "Board config error!", OUTPUT_RED );

		if ( CFG_REGISTER_ERROR == ArduCamCfg_SetRegisterConfig( m_cUsbCameraCfgHd, m_cUsbCameraHd ) )
			InsertText( "Register config error!", OUTPUT_RED );

		m_btPlay.EnableWindow( TRUE );
		m_btStop.EnableWindow( FALSE );
		m_btShot.EnableWindow( TRUE );
		
		m_btInit.EnableWindow( FALSE );
		m_btOpen.EnableWindow( FALSE );
		m_btClose.EnableWindow( TRUE );

		m_btRegRead.EnableWindow( TRUE );
		m_btRegWrite.EnableWindow( TRUE );

		( ( CButton* )GetDlgItem( IDC_RADIO_DISPLAY    ) )->EnableWindow( TRUE );
		( ( CButton* )GetDlgItem( IDC_RADIO_SAVE_DATA  ) )->EnableWindow( TRUE );
		( ( CButton* )GetDlgItem( IDC_RADIO_SAVE_IMAGE ) )->EnableWindow( TRUE );
		( ( CButton* )GetDlgItem( IDC_RADIO_SAVE_VIDEO ) )->EnableWindow( TRUE );

		m_chkIRcut.EnableWindow( TRUE );
		m_chkIRcut.SetCheck( FALSE );

		//if ( m_u8WhiteBalanceCfgEn == 0x7 )		m_chkWhiteBalance.EnableWindow( TRUE );

		if ( stTmpUsbCameraCfg.emImageFmtMode != FORMAT_MODE_JPG )
		{
			m_chkForceDisp.EnableWindow( TRUE );
			m_chkForceDisp.SetCheck( FALSE );
		}

		m_u32FrameReadThreadEn = THREAD_ENABLE;												
		m_ptdFrameReadThread   = AfxBeginThread( _FrameReadThread, this );

		Uint8 u8TmpData[16];
		ArduCam_readUserData( m_cUsbCameraHd, 0x400-16, 16, u8TmpData );

		csTmpString.Format( "Serial: %c%c%c%c-%c%c%c%c-%c%c%c%c",	
							u8TmpData[0], u8TmpData[1], u8TmpData[2], u8TmpData[3], 
							u8TmpData[4], u8TmpData[5], u8TmpData[6], u8TmpData[7], 
							u8TmpData[8], u8TmpData[9], u8TmpData[10], u8TmpData[11] );

		m_sttSerial.SetWindowText( csTmpString );

		InsertText( "USB camera init success!", OUTPUT_BLUE );
	}
}
  

void CUSBTestDlg::OnBnClickedButtonLoad()
{
	// TODO: Add your control notification handler code here
	CString csTmpFileName; 
	CString tmpFileName;
	m_cmbCfgFileName.GetLBText(m_cmbCfgFileName.GetCurSel(), tmpFileName);

	Int32 s32TmpPathSize = m_csCfgFileName.ReverseFind('\\');
	csTmpFileName = m_csCfgFileName.Left(s32TmpPathSize + 1);
	csTmpFileName = csTmpFileName + tmpFileName;
	//csTmpFileName = csTmpFileName + _T("\\Config\\") + m_caCfgFileList.GetAt(m_cmbCfgFileName.GetCurSel()).cFileName;
	
	if ( CFG_NONE_CONTENT == ArduCamCfg_LoadCameraConfig( m_cUsbCameraCfgHd, csTmpFileName ) )
	{
		InsertText( "Config file Open failed!", OUTPUT_RED );
		return;
	}
	
	/* ----- contral UI ----- */
	m_edtVid.EnableWindow(TRUE);	
	m_edtType.EnableWindow(TRUE);
	m_edtWidth.EnableWindow(TRUE);
	m_edtHeight.EnableWindow(TRUE);
	m_edtI2cAddr.EnableWindow(TRUE);
	m_cmbBits.EnableWindow(TRUE);
	m_cmbI2cMode.EnableWindow(TRUE);
	m_cmbImgFormat.EnableWindow(TRUE);

	CString csTmpString;

	Uint32 u32TmpUserSet = 1;
	Uint32 u32TmpCfgData[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
	
	if ( CFG_CAMERA_OK == ArduCamCfg_SetCameraConfig( m_cUsbCameraCfgHd, CFG_CAMERA_CFGMODE, u32TmpCfgData ) )
	{
		u32TmpUserSet = u32TmpCfgData[0];
	}

	//if ( CFG_CAMERA_OK == ArduCamCfg_SetCameraConfig( m_cUsbCameraCfgHd, CFG_CAMERA_VID, u32TmpCfgData ) )
	//{
	//	csTmpString.Format( "0x%X", u32TmpCfgData[0] );
	//	m_edtVid.SetWindowTextA( csTmpString );
	//	m_edtVid.EnableWindow(FALSE);
	//}
	//else
	//	InsertText( "Not find camera VID configuration data!", OUTPUT_RED );

	if ( CFG_CAMERA_OK == ArduCamCfg_SetCameraConfig( m_cUsbCameraCfgHd, CFG_CAMERA_TYPE, u32TmpCfgData ) )
	{
		m_edtType.SetWindowTextA( ( Int8* )u32TmpCfgData );
		m_edtType.EnableWindow(FALSE);
	}
	else
		InsertText( "Not find camera type configuration data!", OUTPUT_RED );

	if ( CFG_CAMERA_OK == ArduCamCfg_SetCameraConfig( m_cUsbCameraCfgHd, CFG_CAMERA_SIZE, u32TmpCfgData ) )
	{
		csTmpString.Format( "%d", u32TmpCfgData[0] );
		m_edtWidth.SetWindowTextA( csTmpString );
		if ( u32TmpCfgData[0] <= MAX_IMAGE_WIDTH )	m_edtWidth.EnableWindow(FALSE);
		else										InsertText( "Image width load out of bounds!", OUTPUT_RED );
		csTmpString.Format( "%d", u32TmpCfgData[1] );
		m_edtHeight.SetWindowTextA( csTmpString );
		if ( u32TmpCfgData[1] <= MAX_IMAGE_HEIGHT )	m_edtHeight.EnableWindow(FALSE);			
		else										InsertText( "Image height load out of bounds!", OUTPUT_RED );
	}
	else if ( CFG_CAMERA_ERROR == ArduCamCfg_SetCameraConfig( m_cUsbCameraCfgHd, CFG_CAMERA_SIZE, u32TmpCfgData ) )
	{
		csTmpString.Format( "%d", u32TmpCfgData[0] );
		m_edtWidth.SetWindowTextA( csTmpString );
		if ( u32TmpCfgData[0] <= MAX_IMAGE_WIDTH )	m_edtWidth.EnableWindow(FALSE);
		else										InsertText( "Image width load out of bounds!", OUTPUT_RED );
		InsertText( "Not find image height configuration data!", OUTPUT_RED );
	}
	else
	{
		InsertText( "Not find image width configuration data!", OUTPUT_RED );
		InsertText( "Not find image height configuration data!", OUTPUT_RED );
	}

	if ( CFG_CAMERA_OK == ArduCamCfg_SetCameraConfig( m_cUsbCameraCfgHd, CFG_CAMERA_BIT_WIDTH, u32TmpCfgData ) )
	{
		switch( u32TmpCfgData[0] )
		{
			case 8 :	m_cmbBits.SetCurSel( 0 );	m_cmbBits.EnableWindow(FALSE);			break;
			case 10:	m_cmbBits.SetCurSel( 1 );	m_cmbBits.EnableWindow(FALSE);			break;
			case 12:	m_cmbBits.SetCurSel( 2 );	m_cmbBits.EnableWindow(FALSE);			break;
			case 14:	m_cmbBits.SetCurSel( 3 );	m_cmbBits.EnableWindow(FALSE);			break;
			case 16:	m_cmbBits.SetCurSel( 4 );	m_cmbBits.EnableWindow(FALSE);			break;
			default:	InsertText( "Unsupported image bits number load!", OUTPUT_RED );	break;
		}
	}

	if ( CFG_CAMERA_OK == ArduCamCfg_SetCameraConfig( m_cUsbCameraCfgHd, CFG_CAMERA_FORMAT, u32TmpCfgData ) )
	{
		if ( u32TmpCfgData[0] < m_cmbImgFormat.GetCount() )
		{
			m_cmbImgFormat.SetCurSel( u32TmpCfgData[0] );
			m_cmbImgFormat.EnableWindow(FALSE);
		}
		else
			InsertText( "Unsupported image format load!", OUTPUT_RED );	

		switch ( u32TmpCfgData[0] )
		{
		case 0:
		case 5:
			switch ( u32TmpCfgData[1] )
			{
			case 0:		m_u32RawMode = RAW_RG;		m_btRawMode.SetWindowTextA( "RAWMod(RG)" );		break;
			case 1:		m_u32RawMode = RAW_GR;		m_btRawMode.SetWindowTextA( "RAWMod(GR)" );		break;
			case 2:		m_u32RawMode = RAW_GB;		m_btRawMode.SetWindowTextA( "RAWMod(GB)" );		break;
			case 3:		m_u32RawMode = RAW_BG;		m_btRawMode.SetWindowTextA( "RAWMod(BG)" );		break;
			default:	InsertText( "Unsupported RAW format load!", OUTPUT_RED );					break;
			}
			break;
		case 1:
			switch ( u32TmpCfgData[1] )
			{
			case 0:		m_u32RgbMode = RGB565_RB;	m_btRawMode.SetWindowTextA( "RGBMod(RB)" );		break;
			case 1:		m_u32RgbMode = RGB565_BR;	m_btRawMode.SetWindowTextA( "RGBMod(BR)" );		break;
			default:	InsertText( "Unsupported RGB565 format load!", OUTPUT_RED );				break;
			}
			break;
		case 2:
			switch ( u32TmpCfgData[1] )
			{
			case 0:		m_u32YuvMode = YUV422_YU;	m_btRawMode.SetWindowTextA( "YUVMod(YU)" );		break;
			case 1:		m_u32YuvMode = YUV422_YV;	m_btRawMode.SetWindowTextA( "YUVMod(YV)" );		break;
			case 2:		m_u32YuvMode = YUV422_UY;	m_btRawMode.SetWindowTextA( "YUVMod(UY)" );		break;
			case 3:		m_u32YuvMode = YUV422_VY;	m_btRawMode.SetWindowTextA( "YUVMod(UV)" );		break;
			default:	InsertText( "Unsupported YUV format load!", OUTPUT_RED );					break;
			}
			break;
		case 3:			m_btRawMode.SetWindowTextA( "JPGMod" );							break;
		case 4:
		case 6:			m_btRawMode.SetWindowTextA( "MONOMod" );						break;
		case 7:			m_btRawMode.SetWindowTextA( "TOF" );							break;
		default:	InsertText( "Unsupported image format load!", OUTPUT_RED );			break;
		}
	}
	else if ( CFG_CAMERA_ERROR == ArduCamCfg_SetCameraConfig( m_cUsbCameraCfgHd, CFG_CAMERA_FORMAT, u32TmpCfgData ) )
	{
		if ( u32TmpCfgData[0] < m_cmbImgFormat.GetCount() )
		{
			m_cmbImgFormat.SetCurSel( u32TmpCfgData[0] );
			m_cmbImgFormat.EnableWindow(FALSE);
		}
		else
			InsertText( "Unsupported image format load!", OUTPUT_RED );	
		
		switch ( u32TmpCfgData[0] )
		{
		case 0:	case 5:	 m_u32RawMode = RAW_RG;		m_btRawMode.SetWindowTextA( "RAWMod(RG)" );		break;
		case 1:			 m_u32RgbMode = RGB565_RB;	m_btRawMode.SetWindowTextA( "RGBMod(RB)" );		break;
		case 2:			 m_u32YuvMode = YUV422_YU;	m_btRawMode.SetWindowTextA( "YUVMod(YU)" );		break;
		case 3:										m_btRawMode.SetWindowTextA( "JPGMod" );			break;
		case 4:	case 6:								m_btRawMode.SetWindowTextA( "MONOMod" );		break;
		case 7:										m_btRawMode.SetWindowTextA( "TOF" );			break;
		}
		if ( u32TmpCfgData[0] != 3 )	InsertText( "Image format load incompletely!", OUTPUT_RED );	
	}
	else
		InsertText( "Not find image format configuration data!", OUTPUT_RED );

	if ( CFG_CAMERA_OK == ArduCamCfg_SetCameraConfig( m_cUsbCameraCfgHd, CFG_CAMERA_I2C_MODE, u32TmpCfgData ) )
	{
		if ( u32TmpCfgData[0] < m_cmbI2cMode.GetCount() )
		{
			m_cmbI2cMode.SetCurSel( u32TmpCfgData[0] );
			m_cmbI2cMode.EnableWindow(FALSE);
		}
		else
			InsertText( "Unsupported I2C mode load!", OUTPUT_RED );	
	}
	else
		InsertText( "Not find I2C mode configuration data!", OUTPUT_RED );

	if ( CFG_CAMERA_OK == ArduCamCfg_SetCameraConfig( m_cUsbCameraCfgHd, CFG_CAMERA_I2C_ADDR, u32TmpCfgData ) )
	{			
		csTmpString.Format( "0x%X", u32TmpCfgData[0] );
		m_edtI2cAddr.SetWindowTextA( csTmpString );
		m_edtI2cAddr.EnableWindow(FALSE);
	}
	else
		InsertText( "Not find I2C address configuration data!", OUTPUT_RED );

	if ( CFG_CAMERA_OK == ArduCamCfg_SetCameraConfig( m_cUsbCameraCfgHd, CFG_CAMERA_TRANS_LVL, u32TmpCfgData ) )			
		m_u32TransLvl = u32TmpCfgData[0];
	else
		m_u32TransLvl = 0;

	//m_u8WhiteBalanceCfgEn = 0;
	
	//if ( CFG_CAMERA_OK == ArduCamCfg_SetCameraConfig( m_cUsbCameraCfgHd, CFG_CAMERA_G_GAIN, u32TmpCfgData ) )
	//{
	//	m_stGainGSet.u32Page	 = u32TmpCfgData[0];
	//	m_stGainGSet.u32Addr	 = u32TmpCfgData[1];
	//	m_stGainGSet.u32MinValue = u32TmpCfgData[2];
	//	m_stGainGSet.u32MaxValue = u32TmpCfgData[3];

	//	m_u8WhiteBalanceCfgEn = m_u8WhiteBalanceCfgEn | 0x01;
	//}
	//else
		//InsertText( "G_Gain configuration error!", OUTPUT_RED );

	//if ( CFG_CAMERA_OK == ArduCamCfg_SetCameraConfig( m_cUsbCameraCfgHd, CFG_CAMERA_B_GAIN, u32TmpCfgData ) )
	//{
	//	m_stGainBSet.u32Page	 = u32TmpCfgData[0];
	//	m_stGainBSet.u32Addr	 = u32TmpCfgData[1];
	//	m_stGainBSet.u32MinValue = u32TmpCfgData[2];
	//	m_stGainBSet.u32MaxValue = u32TmpCfgData[3];

	//	m_u8WhiteBalanceCfgEn = m_u8WhiteBalanceCfgEn | 0x02;
	//}
	//else
		//InsertText( "B_Gain configuration error!", OUTPUT_RED );

	//if ( CFG_CAMERA_OK == ArduCamCfg_SetCameraConfig( m_cUsbCameraCfgHd, CFG_CAMERA_R_GAIN, u32TmpCfgData ) )
	//{
	//	m_stGainRSet.u32Page	 = u32TmpCfgData[0];
	//	m_stGainRSet.u32Addr	 = u32TmpCfgData[1];
	//	m_stGainRSet.u32MinValue = u32TmpCfgData[2];
	//	m_stGainRSet.u32MaxValue = u32TmpCfgData[3];

	//	m_u8WhiteBalanceCfgEn = m_u8WhiteBalanceCfgEn | 0x04;
	//}
	//else
		//InsertText( "R_Gain configuration error!", OUTPUT_RED );

	//if ( CFG_CAMERA_OK == ArduCamCfg_SetCameraConfig( m_cUsbCameraCfgHd, CFG_CAMERA_G2_GAIN, u32TmpCfgData ) )
	//{
	//	m_stGainG2Set.u32Page	 = u32TmpCfgData[0];
	//	m_stGainG2Set.u32Addr	 = u32TmpCfgData[1];
	//	m_stGainG2Set.u32MinValue = u32TmpCfgData[2];
	//	m_stGainG2Set.u32MaxValue = u32TmpCfgData[3];
	//}
	//else
		//InsertText( "G2_Gain configuration error!", OUTPUT_RED );

	//if ( CFG_CAMERA_OK == ArduCamCfg_SetCameraConfig( m_cUsbCameraCfgHd, CFG_CAMERA_Y_GAIN, u32TmpCfgData ) )
	//{
	//	m_stGainYSet.u32Page	 = u32TmpCfgData[0];
	//	m_stGainYSet.u32Addr	 = u32TmpCfgData[1];
	//	m_stGainYSet.u32MinValue = u32TmpCfgData[2];
	//	m_stGainYSet.u32MaxValue = u32TmpCfgData[3];

	//	m_u8WhiteBalanceCfgEn = m_u8WhiteBalanceCfgEn | 0x01;
	//}
	//else
		//InsertText( "Y_Gain configuration error!", OUTPUT_RED );

	//if ( CFG_CAMERA_OK == ArduCamCfg_SetCameraConfig( m_cUsbCameraCfgHd, CFG_CAMERA_U_GAIN, u32TmpCfgData ) )
	//{
	//	m_stGainUSet.u32Page	 = u32TmpCfgData[0];
	//	m_stGainUSet.u32Addr	 = u32TmpCfgData[1];
	//	m_stGainUSet.u32MinValue = u32TmpCfgData[2];
	//	m_stGainUSet.u32MaxValue = u32TmpCfgData[3];

	//	m_u8WhiteBalanceCfgEn = m_u8WhiteBalanceCfgEn | 0x02;
	//}
	//else
		//InsertText( "U_Gain configuration error!", OUTPUT_RED );

	//if ( CFG_CAMERA_OK == ArduCamCfg_SetCameraConfig( m_cUsbCameraCfgHd, CFG_CAMERA_V_GAIN, u32TmpCfgData ) )
	//{
	//	m_stGainVSet.u32Page	 = u32TmpCfgData[0];
	//	m_stGainVSet.u32Addr	 = u32TmpCfgData[1];
	//	m_stGainVSet.u32MinValue = u32TmpCfgData[2];
	//	m_stGainVSet.u32MaxValue = u32TmpCfgData[3];

	//	m_u8WhiteBalanceCfgEn = m_u8WhiteBalanceCfgEn | 0x04;
	//}
	//else
		//InsertText( "V_Gain configuration error!", OUTPUT_RED );

	//if ( CFG_CAMERA_OK == ArduCamCfg_SetCameraConfig( m_cUsbCameraCfgHd, CFG_CAMERA_GL_GAIN, u32TmpCfgData ) )			
	//{
	//	m_stGainGLSet.u32Page	 = u32TmpCfgData[0];
	//	m_stGainGLSet.u32Addr	 = u32TmpCfgData[1];
	//	m_stGainGLSet.u32MinValue = u32TmpCfgData[2];
	//	m_stGainGLSet.u32MaxValue = u32TmpCfgData[3];
	//}
	//else
		//InsertText( "GL_Gain configuration error!", OUTPUT_RED );

	if ( u32TmpUserSet == 0 )
	{
		m_edtVid.EnableWindow(TRUE);	
		m_edtType.EnableWindow(TRUE);
		m_edtWidth.EnableWindow(TRUE);
		m_edtHeight.EnableWindow(TRUE);
		m_edtI2cAddr.EnableWindow(TRUE);
		m_cmbBits.EnableWindow(TRUE);
		m_cmbI2cMode.EnableWindow(TRUE);
		m_cmbImgFormat.EnableWindow(TRUE);
	}
}
  

void CUSBTestDlg::OnBnClickedButtonRefresh()
{
	// TODO: Add your control notification handler code here
	m_cmbCfgFileName.ResetContent();

	HANDLE hdFile;  
	WIN32_FIND_DATA fdFileData;
	m_caCfgFileList.RemoveAll();
	hdFile = FindFirstFile( m_csCfgFileName.GetBuffer(), &fdFileData );  
	if ( (Uint32)hdFile != 0xFFFFFFFF )
	{
		m_caCfgFileList.Add( fdFileData );
		while( FindNextFile( hdFile, &fdFileData) )		m_caCfgFileList.Add( fdFileData );  
	}
	else
	{
		m_btCfgLoad.EnableWindow( FALSE );
		return;
	}

	Uint32 u32FileNum = m_caCfgFileList.GetCount();
	for ( Uint32 u32Index = 0; u32Index < u32FileNum; u32Index++ )
	{
		m_cmbCfgFileName.AddString( m_caCfgFileList.GetAt( u32Index ).cFileName );
	}
	m_cmbCfgFileName.SetCurSel( 0 );
	m_btCfgLoad.EnableWindow( TRUE );
}
  

void CUSBTestDlg::OnBnClickedButtonPlay()
{

	if ( m_u8PlayState != STATE_PLAY )
	{		
		m_u8PlayState = STATE_PLAY;
		
		m_u32FrameCaptureThreadEn = THREAD_ENABLE;												
		m_ptdFrameCaptureThread   = AfxBeginThread( _FrameCaptureThread, this );

		m_btPlay.EnableWindow( FALSE );
		m_btShot.EnableWindow(TRUE);
		m_btStop.EnableWindow( TRUE );
		m_btClose.EnableWindow( FALSE );
	}
}
  

void CUSBTestDlg::OnBnClickedButtonStop()
{
	if ( m_u8PlayState != STATE_STOP )
	{
		m_u8PlayState = STATE_STOP;

		m_u32CaptFlag = 1;
			
		m_u32FrameCaptureThreadEn = THREAD_DISABLE;												

		m_u32FrameCaptureCnt    = 0;
		m_u32FrameCaptureCntBak = 0;

		m_btPlay.EnableWindow( TRUE );
		m_btStop.EnableWindow( FALSE );
		m_btShot.EnableWindow(FALSE);
		m_btClose.EnableWindow( TRUE );
	}
}
  

void CUSBTestDlg::OnBnClickedButtonShot()
{
	m_u32ShotTimeFlag = 1;
} 
  

void CUSBTestDlg::OnBnClickedButtonClose()
{
	
	while ( m_u32CaptFlag == 1 )		Sleep(10);
	
	m_u32ReadFlag = 1;

	m_u32FrameReadThreadEn  = THREAD_DISABLE;
	m_u32FrameReadCnt		= 0;
	m_u32FrameReadCntBak    = 0;

	while ( m_u32ReadFlag == 1 )		Sleep(10);

	ArduCam_close( m_cUsbCameraHd );
	m_cUsbCameraHd = NULL;

	m_btPlay.EnableWindow( FALSE );
	m_btStop.EnableWindow( FALSE );
	m_btShot.EnableWindow( FALSE );

	m_btOpen.EnableWindow( TRUE );
	m_btInit.EnableWindow( TRUE );
	m_btClose.EnableWindow( FALSE );
	if (m_ControlDlg != NULL)
		m_ControlDlg->ShowWindow(SW_HIDE);

	m_btRegRead.EnableWindow( FALSE );
	m_btRegWrite.EnableWindow( FALSE );

	( ( CButton* )GetDlgItem( IDC_RADIO_DISPLAY    ) )->EnableWindow( FALSE );
	( ( CButton* )GetDlgItem( IDC_RADIO_SAVE_DATA  ) )->EnableWindow( FALSE );
	( ( CButton* )GetDlgItem( IDC_RADIO_SAVE_IMAGE ) )->EnableWindow( FALSE );
	( ( CButton* )GetDlgItem( IDC_RADIO_SAVE_VIDEO ) )->EnableWindow( FALSE );

	if ( m_u8RecordEn == RECORD_VIDEO )		ArduCamDisp_EndRecordVideo( m_cUsbCameraRecordHd );

	m_u8RecordEn = RECORD_NULL;
	( ( CButton* )GetDlgItem( IDC_RADIO_DISPLAY    ) )->SetCheck( TRUE );
	( ( CButton* )GetDlgItem( IDC_RADIO_SAVE_DATA  ) )->SetCheck( FALSE );
	( ( CButton* )GetDlgItem( IDC_RADIO_SAVE_IMAGE ) )->SetCheck( FALSE );
	( ( CButton* )GetDlgItem( IDC_RADIO_SAVE_VIDEO ) )->SetCheck( FALSE );

	m_chkIRcut.EnableWindow( FALSE );
	m_chkForceDisp.EnableWindow( FALSE );

	m_chkWhiteBalance.EnableWindow( FALSE );
	m_chkWhiteBalance.SetCheck( FALSE );
	m_u8WhiteBalanceEn = 0;

	( ( CButton* )GetDlgItem( IDC_RADIO_MODE_HORIZONTAL ) )->EnableWindow( FALSE );
	( ( CButton* )GetDlgItem( IDC_RADIO_MODE_VERTICAL   ) )->EnableWindow( FALSE );
	KillTimer(DISP_INF_TIMER);
}
 

void CUSBTestDlg::OnBnClickedButtonRegRead()
{
	
	CString csTmpString;
	Uint32 u32TmpAddr;
	Uint32 u32TmpValue;

	m_edtRegAddr.GetWindowText( csTmpString );
	u32TmpAddr = atoi_ex( csTmpString );

	Uint32 u32State = ArduCam_readSensorReg( m_cUsbCameraHd, u32TmpAddr, &u32TmpValue );

	if ( u32State == USB_CAMERA_NO_ERROR )		csTmpString.Format( "0x%X", u32TmpValue );
	else										csTmpString.Format( "error!" );

	m_edtRegValue.SetWindowText( csTmpString );
}
 

void CUSBTestDlg::OnBnClickedButtonRegWrite()
{
	
	CString csTmpString;
	Uint32 u32TmpAddr;
	Uint32 u32TmpValue;

	m_edtRegAddr.GetWindowText( csTmpString );
	u32TmpAddr = atoi_ex( csTmpString );

	m_edtRegValue.GetWindowText( csTmpString );
	u32TmpValue = atoi_ex( csTmpString );

	Uint32 u32State = ArduCam_writeSensorReg( m_cUsbCameraHd, u32TmpAddr, u32TmpValue );

	if ( u32State != USB_CAMERA_NO_ERROR )		csTmpString.Format( "error!" );
	m_edtRegValue.SetWindowText( csTmpString );
}
  

void CUSBTestDlg::OnDestroy()
{
	CDialog::OnDestroy();

	if ( m_u8PlayState != STATE_STOP )
	{
		m_u8PlayState = STATE_STOP;

		m_u32FrameCaptureThreadEn = THREAD_DISABLE;												
		m_u32FrameReadThreadEn    = THREAD_DISABLE;	
	}

	Sleep(500);
}


void CUSBTestDlg::OnBnClickedButtonByteConv()
{
	
	if ( ( m_cmbImgFormat.GetCurSel() == 0 ) || ( m_cmbImgFormat.GetCurSel() == 5 ) )
	{
		if ( m_u32RawMode == RAW_RG )
		{
			m_u32RawMode = RAW_GR;
			m_btRawMode.SetWindowTextA( "RawMod(GR)" );
		}
		else if ( m_u32RawMode == RAW_GR )
		{
			m_u32RawMode = RAW_GB;
			m_btRawMode.SetWindowTextA( "RawMod(GB)" );
		}
		else if ( m_u32RawMode == RAW_GB )
		{
			m_u32RawMode = RAW_BG;
			m_btRawMode.SetWindowTextA( "RawMod(BG)" );
		}
		else
		{
			m_u32RawMode = RAW_RG;
			m_btRawMode.SetWindowTextA( "RawMod(RG)" );
		}
	}
	else if ( ( m_cmbImgFormat.GetCurSel() == 1 ) || ( m_cmbImgFormat.GetCurSel() == 6 ) )
	{
		if ( m_u32RgbMode == RGB565_RB )
		{
			m_u32RgbMode = RGB565_BR;
			m_btRawMode.SetWindowTextA( "RgbMod(BR)" );
		}
		else
		{
			m_u32RgbMode = RGB565_RB;
			m_btRawMode.SetWindowTextA( "RgbMod(RB)" );
		}
	}
	else if ( ( m_cmbImgFormat.GetCurSel() == 2 ) || ( m_cmbImgFormat.GetCurSel() == 7 ) )
	{
		if ( m_u32YuvMode == YUV422_YU )
		{
			m_u32YuvMode = YUV422_YV;
			m_btRawMode.SetWindowTextA( "YuvMod(YV)" );
		}
		else if ( m_u32YuvMode == YUV422_YV )
		{
			m_u32YuvMode = YUV422_UY;
			m_btRawMode.SetWindowTextA( "YuvMod(UY)" );
		}
		else if ( m_u32YuvMode == YUV422_UY )
		{
			m_u32YuvMode = YUV422_VY;
			m_btRawMode.SetWindowTextA( "YuvMod(VY)" );
		}
		else
		{
			m_u32YuvMode = YUV422_YU;
			m_btRawMode.SetWindowTextA( "YuvMod(YU)" );
		}
	}
}
  

void CUSBTestDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	/* ----- 如鼠标位置在显示区域内，读取当前鼠标位置，并保存，用于鼠标弹起时拖动处理	----- */
	/* ----- 如鼠标位置不在显示区域内，将当前鼠标位置置为（0,0），不做拖动处理			----- */
	if (    ( point.x >= m_sttDisplay_rect.left   )
		 && ( point.x <= m_sttDisplay_rect.right  ) 
		 && ( point.y >= m_sttDisplay_rect.top    )
		 && ( point.y <= m_sttDisplay_rect.bottom ) )
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
	if (    ( point.x >= m_sttDisplay_rect.left   )
		 && ( point.x <= m_sttDisplay_rect.right  ) 
		 && ( point.y >= m_sttDisplay_rect.top    )
		 && ( point.y <= m_sttDisplay_rect.bottom ) )
	{
		if ( m_u8PlayState != STATE_PLAY )		return;
		
		ArduCamCfg	stTmpFramePara;
		ArduCam_getSensorCfg( m_cUsbCameraHd, &stTmpFramePara );
		
		m_s32LocXMoved = point.x - m_s32LocXMoved;
		m_s32LocYMoved = point.y - m_s32LocYMoved;

		m_s32locStartX = m_s32locStartX - ( m_s32LocXMoved * m_pixScale );
		m_s32locStartY = m_s32locStartY + ( m_s32LocYMoved * m_pixScale );

		if (    (    ( stTmpFramePara.emImageFmtMode == FORMAT_MODE_RAW_D )
				  || ( stTmpFramePara.emImageFmtMode == FORMAT_MODE_MON_D ) )
			 && (     m_u8DImageDispMode == MODE_HORIZONTAL					) )
		{
			if ( ( m_s32locStartX + m_u32DisplayWidth * m_pixScale ) > stTmpFramePara.u32Width * 2 )
				m_s32locStartX = stTmpFramePara.u32Width * 2 - m_u32DisplayWidth * m_pixScale;
		}
		else
		{
			if ( ( m_s32locStartX + m_u32DisplayWidth * m_pixScale ) > stTmpFramePara.u32Width )
				m_s32locStartX = stTmpFramePara.u32Width - m_u32DisplayWidth * m_pixScale;
		}

		if (    (    ( stTmpFramePara.emImageFmtMode == FORMAT_MODE_RAW_D )
			|| ( stTmpFramePara.emImageFmtMode == FORMAT_MODE_MON_D ) )
			&& (     m_u8DImageDispMode == MODE_VERTICAL			 		    ) )
		{
			if ( ( m_s32locStartY + m_u32DisplayHeight * m_pixScale ) > stTmpFramePara.u32Height * 2 )
				m_s32locStartY = stTmpFramePara.u32Height * 2 - m_u32DisplayHeight * m_pixScale;
		}
		else
		{
			if ( ( m_s32locStartY + m_u32DisplayHeight * m_pixScale ) > stTmpFramePara.u32Height )
				m_s32locStartY = stTmpFramePara.u32Height - m_u32DisplayHeight * m_pixScale;
		}

		//m_s32locStartX = max( 0, m_s32locStartX );
		//m_s32locStartY = max( 0, m_s32locStartY );

		FillBlackDisplay();
		
		if ( stTmpFramePara.emImageFmtMode == FORMAT_MODE_JPG )		FrameDisplayJPG( m_pu8OutRgb24, m_u32JpgSize );
		else														
		{
			if (    ( stTmpFramePara.emImageFmtMode == FORMAT_MODE_RAW_D )
				 || ( stTmpFramePara.emImageFmtMode == FORMAT_MODE_MON_D ) )
			{
				if ( m_u8DImageDispMode == MODE_HORIZONTAL )
				{
					FrameDisplayBMP( m_pu8OutRgb24, stTmpFramePara.u32Width * 2, stTmpFramePara.u32Height );
				} 
				else
				{
					FrameDisplayBMP( m_pu8OutRgb24, stTmpFramePara.u32Width, stTmpFramePara.u32Height * 2 );
				}
			}
			else
			{
				FrameDisplayBMP( m_pu8OutRgb24, stTmpFramePara.u32Width, stTmpFramePara.u32Height );
			}
		}
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
	/* ----- 将屏幕位置转换为控件内的位置 ----- */
	ScreenToClient( &pt );

	/* ----- 如鼠标位置在显示区域内，读取滚轮转动的值，调整显示范围的大小（增加边界判断） ----- */
	if (    ( pt.x >= m_sttDisplay_rect.left   )
		&& ( pt.x <= m_sttDisplay_rect.right  ) 
		&& ( pt.y >= m_sttDisplay_rect.top    )
		&& ( pt.y <= m_sttDisplay_rect.bottom ) )
	{	
		if ( m_u8PlayState != STATE_PLAY )			return CDialog::OnMouseWheel(nFlags, zDelta, pt);
				
		ArduCamCfg	stTmpFramePara;
		ArduCam_getSensorCfg( m_cUsbCameraHd, &stTmpFramePara );
		
		if ( m_s32ScaleIndex > zDelta / 120 )		m_s32ScaleIndex = min( 17, m_s32ScaleIndex - zDelta / 120 );
		else										m_s32ScaleIndex = 0;

		switch ( m_s32ScaleIndex )
		{
			case 17 :	m_pixScale = 10.0;		break;
			case 16 :	m_pixScale = 8.0;		break;
			case 15 :	m_pixScale = 6.0;		break;
			case 14 :	m_pixScale = 4.0;		break;
			case 13 :	m_pixScale = 3.0;		break;
			case 12 :	m_pixScale = 2.0;		break;
			case 11 :	m_pixScale = 1.5;		break;
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

		if (    (    ( stTmpFramePara.emImageFmtMode == FORMAT_MODE_RAW_D )
			      || ( stTmpFramePara.emImageFmtMode == FORMAT_MODE_MON_D ) )
			 && (     m_u8DImageDispMode == MODE_HORIZONTAL					) )
		{
			if ( ( m_s32locStartX + m_u32DisplayWidth * m_pixScale ) > stTmpFramePara.u32Width * 2 )
				m_s32locStartX = stTmpFramePara.u32Width * 2 - m_u32DisplayWidth * m_pixScale;
		}
		else
		{
			if ( ( m_s32locStartX + m_u32DisplayWidth * m_pixScale ) > stTmpFramePara.u32Width )
				m_s32locStartX = stTmpFramePara.u32Width - m_u32DisplayWidth * m_pixScale;
		}

		if (    (    ( stTmpFramePara.emImageFmtMode == FORMAT_MODE_RAW_D )
			      || ( stTmpFramePara.emImageFmtMode == FORMAT_MODE_MON_D ) )
			&& (     m_u8DImageDispMode == MODE_VERTICAL			 		    ) )
		{
			if ( ( m_s32locStartY + m_u32DisplayHeight * m_pixScale ) > stTmpFramePara.u32Height * 2 )
				m_s32locStartY = stTmpFramePara.u32Height * 2 - m_u32DisplayHeight * m_pixScale;
		}
		else
		{
			if ( ( m_s32locStartY + m_u32DisplayHeight * m_pixScale ) > stTmpFramePara.u32Height )
				m_s32locStartY = stTmpFramePara.u32Height - m_u32DisplayHeight * m_pixScale;
		}

		m_s32locStartX = max( 0, m_s32locStartX );
		m_s32locStartY = max( 0, m_s32locStartY );
		
		FillBlackDisplay();

		if ( stTmpFramePara.emImageFmtMode == FORMAT_MODE_JPG )		FrameDisplayJPG( m_pu8OutRgb24, m_u32JpgSize );
		else
		{
			if (    ( stTmpFramePara.emImageFmtMode == FORMAT_MODE_RAW_D )
				 || ( stTmpFramePara.emImageFmtMode == FORMAT_MODE_MON_D ) )
			{
				if ( m_u8DImageDispMode == MODE_HORIZONTAL )
				{
					FrameDisplayBMP( m_pu8OutRgb24, stTmpFramePara.u32Width * 2, stTmpFramePara.u32Height );
				} 
				else
				{
					FrameDisplayBMP( m_pu8OutRgb24, stTmpFramePara.u32Width, stTmpFramePara.u32Height * 2 );
				}
			}
			else
			{
				FrameDisplayBMP( m_pu8OutRgb24, stTmpFramePara.u32Width, stTmpFramePara.u32Height );
			}
		}
	}

	return CDialog::OnMouseWheel(nFlags, zDelta, pt);
}
   

void CUSBTestDlg::OnBnClickedRadioDisplay()
{
	// TODO: Add your control notification handler code here
	if ( m_u8RecordEn == RECORD_VIDEO )		m_u8RecordEnd = RECORD_NULL;
		//ArduCamDisp_EndRecordVideo( m_cUsbCameraRecordHd );
	else									m_u8RecordEn  = RECORD_NULL;
}


void CUSBTestDlg::OnBnClickedRadioSaveData()
{
	// TODO: Add your control notification handler code here
	if ( m_u8RecordEn == RECORD_VIDEO )		m_u8RecordEnd = RECORD_DATA;
	//ArduCamDisp_EndRecordVideo( m_cUsbCameraRecordHd );
	else									m_u8RecordEn  = RECORD_DATA;

	SYSTEMTIME tiTmpTime;
	GetLocalTime( &tiTmpTime );

	m_csRecordSubPath.Format(  "\\DAT_%04d%02d%02d_%02d%02d%02d", tiTmpTime.wYear, tiTmpTime.wMonth, tiTmpTime.wDay,
																  tiTmpTime.wHour, tiTmpTime.wMinute, tiTmpTime.wSecond );
	if ( !PathIsDirectory( m_csRecordPath + m_csRecordSubPath ) )	CreateDirectory( m_csRecordPath + m_csRecordSubPath, NULL );
}


void CUSBTestDlg::OnBnClickedRadioSaveImage()
{
	// TODO: Add your control notification handler code here
	if ( m_u8RecordEn == RECORD_VIDEO )		m_u8RecordEnd = RECORD_IMAGE;
	//ArduCamDisp_EndRecordVideo( m_cUsbCameraRecordHd );
	else									m_u8RecordEn  = RECORD_IMAGE;

	SYSTEMTIME tiTmpTime;
	GetLocalTime( &tiTmpTime );

	m_csRecordSubPath.Format(  "\\IMG_%04d%02d%02d_%02d%02d%02d", tiTmpTime.wYear, tiTmpTime.wMonth, tiTmpTime.wDay,
															      tiTmpTime.wHour, tiTmpTime.wMinute, tiTmpTime.wSecond );
	if ( !PathIsDirectory( m_csRecordPath + m_csRecordSubPath ) )	CreateDirectory( m_csRecordPath + m_csRecordSubPath, NULL );
}


void CUSBTestDlg::OnBnClickedRadioSaveVideo()
{
	// TODO: Add your control notification handler code here
	if ( m_u8RecordEn == RECORD_VIDEO )			return;
	
	m_u8RecordEn  = RECORD_VIDEO;
	m_u8RecordEnd = RECORD_VIDEO;

	ArduCamDisp_StartRecordVideo( m_cUsbCameraRecordHd, m_csRecordPath );
}
  
  
void CUSBTestDlg::OnBnClickedCheckFullScreen()
{
	// TODO: Add your control notification handler code here
	if ( m_chkFullScreen.GetCheck() == TRUE )			m_u8FullScreenEn = 1;
	else												m_u8FullScreenEn = 0;

	if ( ( m_cUsbCameraHd != NULL ) && ( m_u8PlayState == STATE_PLAY ) )
	{
		ArduCamCfg	stTmpFramePara;
		ArduCam_getSensorCfg( m_cUsbCameraHd, &stTmpFramePara );

		FillBlackDisplay();

		if ( stTmpFramePara.emImageFmtMode == FORMAT_MODE_JPG )		FrameDisplayJPG( m_pu8OutRgb24, m_u32JpgSize );
		else														FrameDisplayBMP( m_pu8OutRgb24, stTmpFramePara.u32Width, stTmpFramePara.u32Height );
	}
}
  

void CUSBTestDlg::OnBnClickedCheckIrcut()
{
	// TODO: Add your control notification handler code here
	if ( m_cUsbCameraHd != NULL )
	{
		Uint32 u32TmpData;
		Uint8 data = { 0 };
		Uint8 pu8DevUsbType;
		Uint8 pu8InfUsbType;
		ArduCam_getUsbType(m_cUsbCameraHd, &pu8DevUsbType, &pu8InfUsbType);
		if (m_chkIRcut.GetCheck() == TRUE)
		{
			if (pu8DevUsbType == USB_3) {
				ArduCam_setboardConfig(m_cUsbCameraHd, 0xF4, 0x01, 0, 0, (Uint8*)&data);
			}
			else {
				ArduCam_readReg_8_8(m_cUsbCameraHd, USB_CPLD_I2C_ADDRESS, 0x01, &u32TmpData);
				u32TmpData = u32TmpData | 0x10;
				ArduCam_writeReg_8_8(m_cUsbCameraHd, USB_CPLD_I2C_ADDRESS, 0x01, u32TmpData);
			}
		}
		else
		{
			if (pu8DevUsbType == USB_3) {
				ArduCam_setboardConfig(m_cUsbCameraHd, 0xF4, 0x09, 0, 0, (Uint8*)&data);
			}
			else {
				ArduCam_readReg_8_8(m_cUsbCameraHd, USB_CPLD_I2C_ADDRESS, 0x01, &u32TmpData);
				u32TmpData = u32TmpData & 0xEF;
				ArduCam_writeReg_8_8(m_cUsbCameraHd, USB_CPLD_I2C_ADDRESS, 0x01, u32TmpData);
			}
		}
	}
}
  

void CUSBTestDlg::OnBnClickedCheckWhiteBalance()
{
	// TODO: Add your control notification handler code here
	if ( m_cUsbCameraHd != NULL )
	{
		if ( m_chkWhiteBalance.GetCheck() == TRUE )
		{
			m_u8WhiteBalanceEn = 1;
		}
		else
		{
			m_u8WhiteBalanceEn = 0;
		}
	}
}
  

#ifdef	FORCE_DISPLAY
void CUSBTestDlg::OnBnClickedCheck1()
{
	
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
#endif


void CUSBTestDlg::OnBnClickedRadioModeHorizontal()
{
	// TODO: Add your control notification handler code here
	m_u8DImageDispMode = MODE_HORIZONTAL;
}


void CUSBTestDlg::OnBnClickedRadioModeVertical()
{
	// TODO: Add your control notification handler code here
	m_u8DImageDispMode = MODE_VERTICAL;
}

  


BOOL CUSBTestDlg::PreTranslateMessage(MSG* pMsg)
{
	/*if (m_tooltip.m_hWnd != NULL)
		m_tooltip.RelayEvent(pMsg);*/
	if (pMsg->message == WM_MOUSEMOVE)
		m_tooltip.RelayEvent(pMsg);

	if ((pMsg->message == WM_KEYDOWN) && (pMsg->wParam == VK_RETURN))
	{
		if (GetFocus() == GetDlgItem(IDC_EDIT4))
		{
			if (m_btRegRead.IsWindowEnabled())
				OnBnClickedButtonRegRead();
		}
		if (GetFocus() == GetDlgItem(IDC_EDIT5))
		{
			if (m_btRegWrite.IsWindowEnabled())
				OnBnClickedButtonRegWrite();
		}
	}


	return CDialog::PreTranslateMessage(pMsg);
}

void CUSBTestDlg::OnMouseMove(UINT nFlags, CPoint point) {
	CString text;
	m_edtType.GetWindowTextA(text);
	m_tooltip.UpdateTipText(text, &m_edtType);
	CDialog::OnMouseMove(nFlags, point);
}
void CUSBTestDlg::readUsbVersion(CString &str) {
	Uint8 pu8DevUsbType;
	Uint8 pu8InfUsbType;
	ArduCam_getUsbType(m_cUsbCameraHd, &pu8DevUsbType, &pu8InfUsbType);
	str.Format(_T("Device: %d\tInterface: %d"), pu8DevUsbType, pu8InfUsbType);
}

BOOL haveCpld(ArduCamHandle useHandle) {
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
void CUSBTestDlg::readCpldVersion(CString &str) {
	Uint32 version;
	Uint8 pu8DevUsbType;
	Uint8 pu8InfUsbType;
	ArduCam_getUsbType(m_cUsbCameraHd, &pu8DevUsbType, &pu8InfUsbType);
	if (pu8DevUsbType == USB_3 && !haveCpld(m_cUsbCameraHd)) {
		str.Format(_T("----/--/--"));
		return;
	}
	ArduCam_readReg_8_8(m_cUsbCameraHd,USB_CPLD_I2C_ADDRESS, 00, &version);
	if (version < 0x23) {
		str.Format(_T("----/--/--"));
		return; 
	}

	Uint32 year, mouth, day, ret_val = 0;
	ArduCam_readReg_8_8(m_cUsbCameraHd,USB_CPLD_I2C_ADDRESS, 5, &year);
	ArduCam_readReg_8_8(m_cUsbCameraHd,USB_CPLD_I2C_ADDRESS, 6, &mouth);
	ArduCam_readReg_8_8(m_cUsbCameraHd,USB_CPLD_I2C_ADDRESS, 7, &day);
	str.Format(_T("V%d.%d\t20%d/%02d/%02d"), (version & 0xF0) >> 4,version & 0x0F, year, mouth, day);
}
void CUSBTestDlg::OnMenuAbout()
{
	AboutDlg aboutDlg;
	if (m_u32FrameReadThreadEn == THREAD_ENABLE && m_cUsbCameraHd) {
		CString str;
		readCpldVersion(str);
		aboutDlg.setCpldVersion(str);
		readUsbVersion(str);
		aboutDlg.setUsbVersion(str);
	}
	else {
		aboutDlg.setCpldVersion("----/--/--");
		aboutDlg.setUsbVersion("Device: --\tInterface: --");
	}
	aboutDlg.DoModal();
}


void CUSBTestDlg::OnOK()
{
	// CDialog::OnOK();
}


void CUSBTestDlg::OnEnChangeEdit4()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialog::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
}


void CUSBTestDlg::OnBnClickedOpenFile()
{
	// TODO: 在此添加控件通知处理程序代码
	TCHAR szFilters[] = _T("All Files (*.*)|*.*|Cfg Files (*.cfg)|*.cfg||");
	// Create an Open dialog; the default file name extension is ".my".
	CFileDialog fileDlg(TRUE, NULL, NULL,
		OFN_FILEMUSTEXIST, szFilters);

	// Display the file dialog. When user clicks OK, fileDlg.DoModal() 
	// returns IDOK.
	if (fileDlg.DoModal() == IDOK)
	{
		CString pathName = fileDlg.GetFolderPath();
		m_csCfgFileName = pathName + _T("\\*.cfg");
		OnBnClickedButtonRefresh();
		m_cmbCfgFileName.SetCurSel(m_cmbCfgFileName.SelectString(0, fileDlg.GetFileName()));
	}
}

BOOL CUSBTestDlg::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	// TODO: 在此添加专用代码和/或调用基类
	MSGFILTER * lpMsgFilter = (MSGFILTER *)lParam;

	if ((wParam == IDC_RICHEDIT_LOG) && (lpMsgFilter->nmhdr.code == EN_MSGFILTER)
		&& (lpMsgFilter->msg == WM_RBUTTONDOWN))

	{
		//if we get through here, we have trapped the right click event of the richeditctrl! 
		CPoint point;
		::GetCursorPos(&point); //where is the mouse?
		CMenu menu; //lets display out context menu :) 
		UINT dwSelectionMade;
		VERIFY(menu.LoadMenu(IDR_MENU2));
		CMenu *pmenuPopup = menu.GetSubMenu(0);
		ASSERT(pmenuPopup != NULL);
		dwSelectionMade = pmenuPopup->TrackPopupMenu((TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_NONOTIFY | TPM_RETURNCMD),
			point.x, point.y, this
		);

		pmenuPopup->DestroyMenu();
		PostMessage(dwSelectionMade, 0, 0);
	}
	return CDialog::OnNotify(wParam, lParam, pResult);
}

LRESULT CUSBTestDlg::OnSelectAll(WPARAM wParam, LPARAM lParam)
{
	m_reInf.SetSel(0, -1);
	return NULL;
}

LRESULT CUSBTestDlg::OnCopy(WPARAM wParam, LPARAM lParam)
{
	m_reInf.Copy();
	return NULL;
}
LRESULT CUSBTestDlg::OnClearAll(WPARAM wParam, LPARAM lParam)
{
	m_reInf.SetSel(0, -1);
	m_reInf.Clear();
	return NULL;
}

void CUSBTestDlg::OnToolsControls()
{
	if (m_ControlDlg == NULL) {
		m_ControlDlg = new ControlDlg(m_cUsbCameraHd, m_cUsbCameraCfgHd);
		BOOL ret = m_ControlDlg->Create(IDD_CONTROL_DIALOG, this);
		if (!ret)   //Create failed.
			AfxMessageBox(_T("Error creating Dialog"));
	}
	if (m_ControlDlg != NULL) {
		m_ControlDlg->InitWeights(m_cUsbCameraHd, m_cUsbCameraCfgHd);
		m_ControlDlg->ShowWindow(SW_SHOW);
	}
}


void CUSBTestDlg::OnToolsDatainfo()
{
	DataInfoDlg dataInfoDlg;
	if (m_u32FrameReadThreadEn == THREAD_ENABLE && m_cUsbCameraHd) {
		dataInfoDlg.SetUsbCameraHandle(m_cUsbCameraHd);
	}
	dataInfoDlg.DoModal();
}
