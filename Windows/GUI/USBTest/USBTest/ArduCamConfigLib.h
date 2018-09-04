#pragma once

#include "Common.h"

#ifdef	 USBCAMERACFGLIB_EXPORTS
#define  ARDUCAMCFG_API extern "C" _declspec(dllexport)
#else
#define  ARDUCAMCFG_API extern "C" _declspec(dllimport)
#endif

typedef  Uint32* ArduCamCfgHandle;

//ARDUCAMCFG_API Uint32 ArduCamCfg_LoadCameraConfig( ArduCamCfgHandle &useHandle, CStdioFile* pcsfile );
ARDUCAMCFG_API Uint32 ArduCamCfg_LoadCameraConfig( ArduCamCfgHandle &useHandle, CString csFilePath );

ARDUCAMCFG_API Uint32 ArduCamCfg_SetCameraConfig( ArduCamCfgHandle useHandle, Uint32 u32ConfigType, Uint32* pu32ConfigData );
ARDUCAMCFG_API Uint32 ArduCamCfg_SetBoardConfig( ArduCamCfgHandle useHandle, ArduCamHandle cUsbCameraHd );
ARDUCAMCFG_API Uint32 ArduCamCfg_SetRegisterConfig( ArduCamCfgHandle useHandle, ArduCamHandle cUsbCameraHd );