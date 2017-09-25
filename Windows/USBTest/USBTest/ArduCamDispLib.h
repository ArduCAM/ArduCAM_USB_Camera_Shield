#pragma once

#include "Common.h"

#ifdef	 USBCAMERADISPLIB_EXPORTS
#define  ARDUCAMDISP_API extern "C" _declspec(dllexport)
#else
#define  ARDUCAMDISP_API extern "C" _declspec(dllimport)
#endif

#define DISP_PROC_OK			0
#define DISP_PROC_ERROR			0xFFFFFFFF

ARDUCAMDISP_API Uint32 ArduCamDisp_Raw2rgb24( Uint8 *pu8DstBmpData, Uint8 *pu8ResRawData, Uint32 u32Width, Uint32 u32Height, Uint8 u8RawMode, Uint8 u8PixelBits );
ARDUCAMDISP_API Uint32 ArduCamDisp_Yuv4222rgb24( Uint8* pu8DstBmpData, Uint8* pu8ResYuvData, Uint32 u32Width, Uint32 u32Height, Uint8 u8YuvMode );
ARDUCAMDISP_API Uint32 ArduCamDisp_Rgb5652rgb24( Uint8* pu8DstBmpData, Uint8* pu8ResRgbData, Uint32 u32Width, Uint32 u32Height, Uint8 u8RgbMode );

ARDUCAMDISP_API void ArduCamDisp_FrameDisplayBMP( CDC *pcdDC, CRect* pcrRect, Uint8* pu8RgbData, Uint32 u32Width, Uint32 u32Height, 
	Uint32 u32DispWidth, Uint32 u32DispHeight, Int32 s32locStartX, Int32 s32locStartY, 
	f64 f64PixScale, Uint8 u8FullScreenEn );

ARDUCAMDISP_API void ArduCamDisp_FrameDisplayJPG( CDC *pcdDC, CRect* pcrRect, Uint8* pu8JpgData, Uint32 u32JpgSize,
	Uint32 u32DispWidth, Uint32 u32DispHeight, Int32 s32locStartX, Int32 s32locStartY, 
	f64 f64PixScale, Uint8 u8FullScreenEn );


ARDUCAMDISP_API void ArduCamDisp_RecordImageBMP( CString csFileName, Uint8* pu8BmpData, Uint32 u32Width, Uint32 u32Height );
ARDUCAMDISP_API void ArduCamDisp_RecordImageJPG( CString csFileName, Uint8* pu8JpgData, Uint32 u32JpgSize );
ARDUCAMDISP_API void ArduCamDisp_RecordImageData( CString csFileName, Uint8* pu8Data, Uint32 u32Size );
