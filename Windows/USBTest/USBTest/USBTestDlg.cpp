// USBTestDlg.cpp : 
//

#include "stdafx.h"
#include "USBTest.h"
#include "USBTestDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


UINT _FrameCaptureThread( LPVOID lParam );						
UINT _FrameReadThread( LPVOID lParam );								

CUSBTestDlg::CUSBTestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CUSBTestDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_cUsbCameraHd = NULL;

	GetModuleFileName( NULL, m_csCfgFileName.GetBuffer(512), 512 );									
	m_csCfgFileName.ReleaseBuffer();
	Int32 s32TmpPathSize = m_csCfgFileName.ReverseFind( '\\' );	
	m_csCfgFileName	= m_csCfgFileName.Left( s32TmpPathSize );	

	m_csCfgFileName = m_csCfgFileName + _T("\\Config\\*.cfg");
	
	m_pu8OutRgb24 = ( Uint8* )malloc( 4096 * 4096 * 3 );

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
END_MESSAGE_MAP()



BOOL CUSBTestDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetIcon(m_hIcon, TRUE);			
	SetIcon(m_hIcon, FALSE);		

#ifndef FORCE_DISPLAY
	m_chkForceDisp.ShowWindow(FALSE);
#endif
	
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
	m_u8RecordEn	     = RECORD_NULL;
	m_u32RecordIndex     = 0;

	m_chkForceDisp.EnableWindow( FALSE );

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

	SetTimer( DISP_INF_TIMER, 1000, NULL );

	m_u32CaptFlag  = 0;
	
	m_s32LocXMoved = 0xFFFF;
	m_s32LocYMoved = 0xFFFF;

	m_s32locStartX = 0;
	m_s32locStartY = 0;

	m_pixScale = 1;
	m_s32ScaleIndex = 10;

	DlgFrameSet();

	FillBlackDisplay();

	m_u8FullScreenEn  = 0;

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
	m_cmbCfgFileName.GetWindowRect(&m_btScan_rect);
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
	// TODO: ÔÚ´ËÌí¼ÓÏûÏ¢´¦Àí³ÌÐò´úÂëºÍ/»òµ÷ÓÃÄ¬ÈÏÖµ
	switch (nIDEvent)
	{
		/* --------------------------------------------------------------------------------------------------- */
		/* ----- ×Ô¼º±àÐ´µÄ²¿·Ö																		 ----- */
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

	csTmpString.Format( "Capt: %d fps", m_u32FrameCaptureCnt - m_u32FrameCaptureCntBak );
	m_sttFrameCaptureCnt.SetWindowText( csTmpString );

	csTmpString.Format( "Read: %d fps", m_u32FrameReadCnt - m_u32FrameReadCntBak );
	m_sttFrameReadCnt.SetWindowText( csTmpString );

	m_u32FrameCaptureCntBak = m_u32FrameCaptureCnt;
	m_u32FrameReadCntBak    = m_u32FrameReadCnt;
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
		if ( ArduCam_availiableImage( m_cUsbCameraHd ) > 0 )
		{
			u32TmpState = ArduCam_readImage( m_cUsbCameraHd, pstTmpFrameData );
			
			if ( u32TmpState == USB_CAMERA_NO_ERROR )
			{
				m_u32FrameReadCnt++;

				CString csTmpString;

				if ( m_u8RecordEn == RECORD_NULL )
				{
					switch ( pstTmpFrameData->stImagePara.emImageFmtMode )
					{
					case FORMAT_MODE_RAW:
						ArduCamDisp_Raw2rgb24( m_pu8OutRgb24, pstTmpFrameData->pu8ImageData, pstTmpFrameData->stImagePara.u32Width, pstTmpFrameData->stImagePara.u32Height, m_u32RawMode, pstTmpFrameData->stImagePara.u8PixelBits );
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
					}
				}
				else if ( m_u8RecordEn == RECORD_DATA )
				{
					csTmpString = CreateDataFileName( &( pstTmpFrameData->stImagePara ) ); 
					ArduCamDisp_RecordImageData( csTmpString, pstTmpFrameData->pu8ImageData, pstTmpFrameData->stImagePara.u32Size );
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
				
	return;
}
  













