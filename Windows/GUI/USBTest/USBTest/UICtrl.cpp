#include "stdafx.h"
#include "USBTest.h"
#include "USBTestDlg.h"


void CUSBTestDlg::OnBnClickedButtonScan()
{
	// TODO: 在此添加控件通知处理程序代码
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
	// TODO: 在此添加控件通知处理程序代码
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

		if ( m_u8WhiteBalanceCfgEn == 0x7 )		m_chkWhiteBalance.EnableWindow( TRUE );

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

	switch ( m_cmbImgFormat.GetCurSel() )
	{
		case 0:		stTmpUsbCameraCfg.emImageFmtMode = FORMAT_MODE_RAW;		break;
		case 1:		stTmpUsbCameraCfg.emImageFmtMode = FORMAT_MODE_RGB;		break;
		case 2:		stTmpUsbCameraCfg.emImageFmtMode = FORMAT_MODE_YUV;		break;
		case 3:		stTmpUsbCameraCfg.emImageFmtMode = FORMAT_MODE_JPG;		break;
		case 4:		stTmpUsbCameraCfg.emImageFmtMode = FORMAT_MODE_MON;		break;
		case 5:		stTmpUsbCameraCfg.emImageFmtMode = FORMAT_MODE_RAW_D;	break;
		case 6:		stTmpUsbCameraCfg.emImageFmtMode = FORMAT_MODE_MON_D;	break;
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

		if ( m_u8WhiteBalanceCfgEn == 0x7 )		m_chkWhiteBalance.EnableWindow( TRUE );

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

	GetModuleFileName( NULL, csTmpFileName.GetBuffer(512), 512 );									
	csTmpFileName.ReleaseBuffer();
	Int32 s32TmpPathSize = csTmpFileName.ReverseFind( '\\' );	
	csTmpFileName	= csTmpFileName.Left( s32TmpPathSize );	

	csTmpFileName = csTmpFileName + _T("\\Config\\") + m_caCfgFileList.GetAt( m_cmbCfgFileName.GetCurSel() ).cFileName;

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

	m_u8WhiteBalanceCfgEn = 0;
	
	if ( CFG_CAMERA_OK == ArduCamCfg_SetCameraConfig( m_cUsbCameraCfgHd, CFG_CAMERA_G_GAIN, u32TmpCfgData ) )
	{
		m_stGainGSet.u32Page	 = u32TmpCfgData[0];
		m_stGainGSet.u32Addr	 = u32TmpCfgData[1];
		m_stGainGSet.u32MinValue = u32TmpCfgData[2];
		m_stGainGSet.u32MaxValue = u32TmpCfgData[3];

		m_u8WhiteBalanceCfgEn = m_u8WhiteBalanceCfgEn | 0x01;
	}
	//else
		//InsertText( "G_Gain configuration error!", OUTPUT_RED );

	if ( CFG_CAMERA_OK == ArduCamCfg_SetCameraConfig( m_cUsbCameraCfgHd, CFG_CAMERA_B_GAIN, u32TmpCfgData ) )
	{
		m_stGainBSet.u32Page	 = u32TmpCfgData[0];
		m_stGainBSet.u32Addr	 = u32TmpCfgData[1];
		m_stGainBSet.u32MinValue = u32TmpCfgData[2];
		m_stGainBSet.u32MaxValue = u32TmpCfgData[3];

		m_u8WhiteBalanceCfgEn = m_u8WhiteBalanceCfgEn | 0x02;
	}
	//else
		//InsertText( "B_Gain configuration error!", OUTPUT_RED );

	if ( CFG_CAMERA_OK == ArduCamCfg_SetCameraConfig( m_cUsbCameraCfgHd, CFG_CAMERA_R_GAIN, u32TmpCfgData ) )
	{
		m_stGainRSet.u32Page	 = u32TmpCfgData[0];
		m_stGainRSet.u32Addr	 = u32TmpCfgData[1];
		m_stGainRSet.u32MinValue = u32TmpCfgData[2];
		m_stGainRSet.u32MaxValue = u32TmpCfgData[3];

		m_u8WhiteBalanceCfgEn = m_u8WhiteBalanceCfgEn | 0x04;
	}
	//else
		//InsertText( "R_Gain configuration error!", OUTPUT_RED );

	if ( CFG_CAMERA_OK == ArduCamCfg_SetCameraConfig( m_cUsbCameraCfgHd, CFG_CAMERA_G2_GAIN, u32TmpCfgData ) )
	{
		m_stGainG2Set.u32Page	 = u32TmpCfgData[0];
		m_stGainG2Set.u32Addr	 = u32TmpCfgData[1];
		m_stGainG2Set.u32MinValue = u32TmpCfgData[2];
		m_stGainG2Set.u32MaxValue = u32TmpCfgData[3];
	}
	//else
		//InsertText( "G2_Gain configuration error!", OUTPUT_RED );

	if ( CFG_CAMERA_OK == ArduCamCfg_SetCameraConfig( m_cUsbCameraCfgHd, CFG_CAMERA_Y_GAIN, u32TmpCfgData ) )
	{
		m_stGainYSet.u32Page	 = u32TmpCfgData[0];
		m_stGainYSet.u32Addr	 = u32TmpCfgData[1];
		m_stGainYSet.u32MinValue = u32TmpCfgData[2];
		m_stGainYSet.u32MaxValue = u32TmpCfgData[3];

		m_u8WhiteBalanceCfgEn = m_u8WhiteBalanceCfgEn | 0x01;
	}
	//else
		//InsertText( "Y_Gain configuration error!", OUTPUT_RED );

	if ( CFG_CAMERA_OK == ArduCamCfg_SetCameraConfig( m_cUsbCameraCfgHd, CFG_CAMERA_U_GAIN, u32TmpCfgData ) )
	{
		m_stGainUSet.u32Page	 = u32TmpCfgData[0];
		m_stGainUSet.u32Addr	 = u32TmpCfgData[1];
		m_stGainUSet.u32MinValue = u32TmpCfgData[2];
		m_stGainUSet.u32MaxValue = u32TmpCfgData[3];

		m_u8WhiteBalanceCfgEn = m_u8WhiteBalanceCfgEn | 0x02;
	}
	//else
		//InsertText( "U_Gain configuration error!", OUTPUT_RED );

	if ( CFG_CAMERA_OK == ArduCamCfg_SetCameraConfig( m_cUsbCameraCfgHd, CFG_CAMERA_V_GAIN, u32TmpCfgData ) )
	{
		m_stGainVSet.u32Page	 = u32TmpCfgData[0];
		m_stGainVSet.u32Addr	 = u32TmpCfgData[1];
		m_stGainVSet.u32MinValue = u32TmpCfgData[2];
		m_stGainVSet.u32MaxValue = u32TmpCfgData[3];

		m_u8WhiteBalanceCfgEn = m_u8WhiteBalanceCfgEn | 0x04;
	}
	//else
		//InsertText( "V_Gain configuration error!", OUTPUT_RED );

	if ( CFG_CAMERA_OK == ArduCamCfg_SetCameraConfig( m_cUsbCameraCfgHd, CFG_CAMERA_GL_GAIN, u32TmpCfgData ) )			
	{
		m_stGainGLSet.u32Page	 = u32TmpCfgData[0];
		m_stGainGLSet.u32Addr	 = u32TmpCfgData[1];
		m_stGainGLSet.u32MinValue = u32TmpCfgData[2];
		m_stGainGLSet.u32MaxValue = u32TmpCfgData[3];
	}
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
	// TODO: 在此添加控件通知处理程序代码
	if ( m_u8PlayState != STATE_PLAY )
	{		
		m_u8PlayState = STATE_PLAY;
		
		m_u32FrameCaptureThreadEn = THREAD_ENABLE;												
		m_ptdFrameCaptureThread   = AfxBeginThread( _FrameCaptureThread, this );

		m_btPlay.EnableWindow( FALSE );
		m_btStop.EnableWindow( TRUE );
		m_btClose.EnableWindow( FALSE );
	}
}
  

void CUSBTestDlg::OnBnClickedButtonStop()
{
	// TODO: 在此添加控件通知处理程序代码
	if ( m_u8PlayState != STATE_STOP )
	{
		m_u8PlayState = STATE_STOP;

		m_u32CaptFlag = 1;
			
		m_u32FrameCaptureThreadEn = THREAD_DISABLE;												

		m_u32FrameCaptureCnt    = 0;
		m_u32FrameCaptureCntBak = 0;

		m_btPlay.EnableWindow( TRUE );
		m_btStop.EnableWindow( FALSE );
		m_btClose.EnableWindow( TRUE );
	}
}
  

void CUSBTestDlg::OnBnClickedButtonShot()
{
	// TODO: 在此添加控件通知处理程序代码
	m_u32ShotTimeFlag = 1;
} 
  

void CUSBTestDlg::OnBnClickedButtonClose()
{
	// TODO: 在此添加控件通知处理程序代码
	while ( m_u32CaptFlag == 1 )		Sleep(10);
	
	m_u32ReadFlag = 1;

	m_u32FrameReadThreadEn  = THREAD_DISABLE;
	m_u32FrameReadCnt		= 0;
	m_u32FrameReadCntBak    = 0;

	while ( m_u32ReadFlag == 1 )		Sleep(10);

	ArduCam_close( m_cUsbCameraHd );	m_cUsbCameraHd = NULL;

	m_btPlay.EnableWindow( FALSE );
	m_btStop.EnableWindow( FALSE );
	m_btShot.EnableWindow( FALSE );

	m_btOpen.EnableWindow( TRUE );
	m_btInit.EnableWindow( TRUE );
	m_btClose.EnableWindow( FALSE );

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
}
 

void CUSBTestDlg::OnBnClickedButtonRegRead()
{
	// TODO: 在此添加控件通知处理程序代码
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
	// TODO: 在此添加控件通知处理程序代码
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

	// TODO: 在此处添加消息处理程序代码
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
	// TODO: 在此添加控件通知处理程序代码
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
	// TODO: 在此添加消息处理程序代码和/或调用默认值	
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
	// TODO: 在此添加消息处理程序代码和/或调用默认值
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

		if ( m_chkIRcut.GetCheck() == TRUE )
		{
			ArduCam_readReg_8_8( m_cUsbCameraHd, 0x46, 0x01, &u32TmpData );
			u32TmpData = u32TmpData | 0x10;
			ArduCam_writeReg_8_8( m_cUsbCameraHd, 0x46, 0x01, u32TmpData );
		}
		else
		{
			ArduCam_readReg_8_8( m_cUsbCameraHd, 0x46, 0x01, &u32TmpData );
			u32TmpData = u32TmpData & 0xEF;
			ArduCam_writeReg_8_8( m_cUsbCameraHd, 0x46, 0x01, u32TmpData );
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

  
