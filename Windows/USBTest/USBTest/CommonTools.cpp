#include "stdafx.h"
#include "USBTest.h"
#include "USBTestDlg.h"

Int32 CUSBTestDlg::atoi_ex( CString csUseNum ) 
{
	Uint32	u32Index  = 0;
	Uint32	u32Mode   = 0;
	Int32	s32Result = 0;

	Int8	s8Data[128];
	
	Uint32	u32DataLen = csUseNum.GetLength();	
	strcpy( s8Data, (LPCTSTR)csUseNum );

	/* -- 判断空字符 -- */
	if ( u32DataLen == 0 )			return 0;

	/* -- 判断负号 -- */
	if ( s8Data[0] != '-' )			
	{
		u32Index  = 0;
		s32Result = 1;
	}
	else if ( u32DataLen > 1 )
	{
		u32Index  = 1;
		s32Result = -1;
	}
	else
	{
		return 0;
	}
	
	/* -- 判断16进制符号 -- */
	if ( u32DataLen == ( u32Index + 1 ) )
	{
		u32Mode = 10;
	}
	else if ( ( s8Data[u32Index] == '0' ) && ( ( s8Data[u32Index+1] == 'x') || ( s8Data[u32Index+1] == 'X' ) ) )		
	{
		u32Mode  = 16;
		u32Index = u32Index + 2;
	}
	else
	{
		u32Mode = 10;
	}
	
	/* -- 计算绝对值 -- */
	Int32 s32TmpValue = 0;
	for ( Uint32 u32I = u32Index; u32I < u32DataLen; u32I++ )
	{
		s32TmpValue = s32TmpValue * u32Mode + Asc2Num( s8Data[u32I] );
	}
	
	/* -- 乘以符号 -- */
	s32Result = s32Result * s32TmpValue;

	return s32Result;
}
  

Int8 CUSBTestDlg::Asc2Num( Int8 s8Data )
{
	if ( ( s8Data >= '0' ) && ( s8Data <= '9' )	)		return	s8Data - '0';
	if ( ( s8Data >= 'A' ) && ( s8Data <= 'F' ) )		return	10 + s8Data - 'A';
	if ( ( s8Data >= 'a' ) && ( s8Data <= 'f' ) )		return	10 + s8Data - 'a';

	return 0;
}
 

Int8 CUSBTestDlg::Num2Asc( Int8 s8Data )
{
	if ( ( s8Data >=  0 ) && ( s8Data <=  9 ) )		return	s8Data + '0';
	if ( ( s8Data >= 10 ) && ( s8Data <= 15 ) )		return	s8Data - 10 + 'A';

	return 0;
}


void CUSBTestDlg::FillBlackDisplay( void )
{
	CDC	  *pcdDC;
	CRect crRect;	
	CWnd  *pcwWnd = GetDlgItem( IDC_STATIC_DISPLAY );
	pcwWnd->GetClientRect( &crRect );
	pcdDC = pcwWnd->GetDC();

	BITMAPINFOHEADER bmiHeader;
	bmiHeader.biSize		  = sizeof(BITMAPINFOHEADER);
	bmiHeader.biWidth		  = m_u32DisplayWidth;
	bmiHeader.biHeight		  = m_u32DisplayHeight;
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
				   crRect.left, crRect.top, crRect.left+m_u32DisplayWidth, crRect.top+m_u32DisplayHeight, 
				   0, 0, m_u32DisplayWidth, m_u32DisplayHeight, 
				   NULL, ( BITMAPINFO* )&bmiHeader, BI_RGB, BLACKNESS );

	ReleaseDC( pcdDC );

	return;
}
  

CString CUSBTestDlg::CreateImageFileName( ArduCamCfg* pstUseCfg )
{
	CString csTmpString;

	SYSTEMTIME tiTmpTime;
	GetLocalTime( &tiTmpTime );

	if (    ( tiTmpTime.wYear   != m_tiBackTime.wYear   )
		 || ( tiTmpTime.wMonth  != m_tiBackTime.wMonth  )
		 || ( tiTmpTime.wDay    != m_tiBackTime.wDay    )
		 || ( tiTmpTime.wHour   != m_tiBackTime.wHour   )
		 || ( tiTmpTime.wMinute != m_tiBackTime.wMinute )
		 || ( tiTmpTime.wSecond != m_tiBackTime.wSecond ) )
	{
		m_u32RecordIndex = 0;
		m_tiBackTime = tiTmpTime;
	}

	m_u32RecordIndex++;	
	csTmpString.Format(  "\\%04d%02d%02d_%02d%02d%02d_Image_%03d.", tiTmpTime.wYear, tiTmpTime.wMonth, tiTmpTime.wDay,
																	tiTmpTime.wHour, tiTmpTime.wMinute, tiTmpTime.wSecond,
																	m_u32RecordIndex );

	if ( pstUseCfg->emImageFmtMode == FORMAT_MODE_JPG )		csTmpString = m_csRecordPath + m_csRecordSubPath + csTmpString + "jpg";
	else													csTmpString = m_csRecordPath + m_csRecordSubPath + csTmpString + "bmp";

	return csTmpString;
}
  

CString CUSBTestDlg::CreateDataFileName( ArduCamCfg* pstUseCfg )
{
	CString csTmpString;

	SYSTEMTIME tiTmpTime;
	GetLocalTime( &tiTmpTime );

	if (    ( tiTmpTime.wYear   != m_tiBackTime.wYear   )
		 || ( tiTmpTime.wMonth  != m_tiBackTime.wMonth  )
		 || ( tiTmpTime.wDay    != m_tiBackTime.wDay    )
		 || ( tiTmpTime.wHour   != m_tiBackTime.wHour   )
		 || ( tiTmpTime.wMinute != m_tiBackTime.wMinute )
		 || ( tiTmpTime.wSecond != m_tiBackTime.wSecond ) )
	{
		m_u32RecordIndex = 0;
		m_tiBackTime = tiTmpTime;
	}

	m_u32RecordIndex++;
	csTmpString.Format(  "\\%04d%02d%02d_%02d%02d%02d_%d_%d_%d_%03d.", tiTmpTime.wYear, tiTmpTime.wMonth, tiTmpTime.wDay,
																	   tiTmpTime.wHour, tiTmpTime.wMinute, tiTmpTime.wSecond,
																	   pstUseCfg->u32Width, pstUseCfg->u32Height, pstUseCfg->u8PixelBits, 
																	   m_u32RecordIndex );

	switch ( pstUseCfg->emImageFmtMode )
	{
	case FORMAT_MODE_RAW :	csTmpString = m_csRecordPath + m_csRecordSubPath + csTmpString + "raw";			break;
	case FORMAT_MODE_RGB :	csTmpString = m_csRecordPath + m_csRecordSubPath + csTmpString + "rgb565";		break;
	case FORMAT_MODE_YUV :	csTmpString = m_csRecordPath + m_csRecordSubPath + csTmpString + "yuv422";		break;
	case FORMAT_MODE_JPG :	csTmpString = m_csRecordPath + m_csRecordSubPath + csTmpString + "jpg";			break;
	}

	return csTmpString;
}