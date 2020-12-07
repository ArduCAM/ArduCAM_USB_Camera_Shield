#pragma once

#include "common.h"

#ifdef	 USBCAMERALIB_EXPORTS
#define  ARDUCAM_API extern "C" _declspec(dllexport)
#else
#define  ARDUCAM_API extern "C" _declspec(dllimport)
#endif

ARDUCAM_API Uint32 ArduCam_autoopen( ArduCamHandle &useHandle, ArduCamCfg* useCfg );

ARDUCAM_API Uint32 ArduCam_scan( ArduCamIndexinfo* pstUsbIdxArray );
ARDUCAM_API Uint32 ArduCam_open( ArduCamHandle &useHandle, ArduCamCfg* useCfg, Uint32 usbIdx );
ARDUCAM_API Uint32 ArduCam_close( ArduCamHandle useHandle );

ARDUCAM_API Uint32 ArduCam_beginCaptureImage( ArduCamHandle useHandle );
ARDUCAM_API Uint32 ArduCam_captureImage( ArduCamHandle useHandle );
ARDUCAM_API Uint32 ArduCam_endCaptureImage( ArduCamHandle useHandle );

ARDUCAM_API Uint32 ArduCam_captureIMU( ArduCamHandle useHandle );

ARDUCAM_API Uint32 ArduCam_availableImage( ArduCamHandle useHandle );
ARDUCAM_API Uint32 ArduCam_availableIMU( ArduCamHandle useHandle );
ARDUCAM_API Uint32 ArduCam_readImage( ArduCamHandle useHandle, ArduCamOutData* &pstFrameData );	
//ARDUCAM_API Uint32 ArduCam_readIMU( ArduCamHandle useHandle, Uint16 u16Index, ArduIMUData* &pstIMUData );
//ARDUCAM_API Uint32 ArduCam_readImageAndIMU( ArduCamHandle useHandle, ArduCamOutData* &pstFrameData, ArduIMUData* &pstIMUData );

ARDUCAM_API Uint32 ArduCam_del( ArduCamHandle useHandle );							
ARDUCAM_API Uint32 ArduCam_flush( ArduCamHandle useHandle );

ARDUCAM_API Uint32 ArduCam_writeSensorReg( ArduCamHandle useHandle, Uint32 regAddr, Uint32 val );
ARDUCAM_API Uint32 ArduCam_readSensorReg( ArduCamHandle useHandle, Uint32 regAddr, Uint32* pval );

ARDUCAM_API Uint32 ArduCam_getSensorCfg( ArduCamHandle useHandle, ArduCamCfg* useCfg );
ARDUCAM_API Uint32 ArduCam_getUsbType( ArduCamHandle useHandle, Uint8* pu8DevUsbType, Uint8* pu8InfUsbType );

ARDUCAM_API Uint32 ArduCam_writeReg_8_8( ArduCamHandle useHandle, Uint32 i2cAddr, Uint32 regAddr, Uint32 val );
ARDUCAM_API Uint32 ArduCam_readReg_8_8( ArduCamHandle useHandle, Uint32 i2cAddr, Uint32 regAddr, Uint32* pval );
ARDUCAM_API Uint32 ArduCam_writeReg_8_16( ArduCamHandle useHandle, Uint32 i2cAddr, Uint32 regAddr, Uint32 val );
ARDUCAM_API Uint32 ArduCam_readReg_8_16( ArduCamHandle useHandle, Uint32 i2cAddr, Uint32 regAddr, Uint32* pval );
ARDUCAM_API Uint32 ArduCam_writeReg_16_8( ArduCamHandle useHandle, Uint32 i2cAddr, Uint32 regAddr, Uint32 val );
ARDUCAM_API Uint32 ArduCam_readReg_16_8( ArduCamHandle useHandle, Uint32 i2cAddr, Uint32 regAddr, Uint32* pval );
ARDUCAM_API Uint32 ArduCam_writeReg_16_16( ArduCamHandle useHandle, Uint32 i2cAddr, Uint32 regAddr, Uint32 val );
ARDUCAM_API Uint32 ArduCam_readReg_16_16( ArduCamHandle useHandle, Uint32 i2cAddr, Uint32 regAddr, Uint32* pval );

ARDUCAM_API Uint32 ArduCam_setboardConfig( ArduCamHandle useHandle, Uint8 u8Command, Uint16 u16Value, Uint16 u16Index, Uint32 u32BufSize, Uint8 *pu8Buf );

ARDUCAM_API Uint32 ArduCam_readUserData( ArduCamHandle useHandle, Uint16 u16Addr, Uint8 u8Len, Uint8* pu8Data );
ARDUCAM_API Uint32 ArduCam_writeUserData( ArduCamHandle useHandle, Uint16 u16Addr, Uint8 u8Len, Uint8* pu8Data );

ARDUCAM_API void ArduCam_enableForceRead( ArduCamHandle useHandle );
ARDUCAM_API void ArduCam_disableForceRead( ArduCamHandle useHandle );

ARDUCAM_API Uint32 ArduCam_setFrqDivPara( ArduCamHandle useHandle, Uint8 u8FrqDivPara );

ARDUCAM_API Uint32 ArduCam_softTrigger(ArduCamHandle useHandle);
ARDUCAM_API Uint32 ArduCam_isFrameReady(ArduCamHandle useHandle);
ARDUCAM_API Uint32 ArduCam_getSingleFrame(ArduCamHandle useHandle, ArduCamOutData* &pstFrameData,int time_out = 1500);
ARDUCAM_API Uint32 ArduCam_setMode(ArduCamHandle useHandle, int mode);
ARDUCAM_API Int32 ArduCam_registerCtrls(ArduCamHandle useHandle, Control *controls, Uint32 controls_length);
ARDUCAM_API Int32 ArduCam_setCtrl(ArduCamHandle useHandle, const char *func_name, Int64 val);
