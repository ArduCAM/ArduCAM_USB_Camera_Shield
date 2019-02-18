#pragma once
#pragma pack(8)

#define _AFXDLL

#include <afx.h>  
#include <atlstr.h>  
#include <stdio.h>
#include <stdlib.h>

#include "ArduCamlib.h"

#ifdef	 USBCAMERACFGLIB_EXPORTS
#define  ARDUCAMCFG_API extern "C" _declspec(dllexport)
#else
#define  ARDUCAMCFG_API extern "C" _declspec(dllimport)
#endif

typedef  Uint32* ArduCamCfgHandle;

#define CFG_TITLE_ERROR					0x10
#define CFG_TITLE_CAMERA				0x11
#define CFG_TITLE_BOARD					0x12
#define CFG_TITLE_REGISTER				0x13

#define CFG_TITLE_BOARD_2_2				0x52
#define CFG_TITLE_REGISTER_2_2			0x53
#define CFG_TITLE_BOARD_3_2				0x92
#define CFG_TITLE_REGISTER_3_2			0x93
#define CFG_TITLE_BOARD_3_3				0xD2
#define CFG_TITLE_REGISTER_3_3			0xD3

#define CFG_CAMERA_ERROR				0x1100
#define CFG_CAMERA_CFGMODE				0x1101
#define CFG_CAMERA_TYPE					0x1102
#define CFG_CAMERA_VID					0x1103
#define CFG_CAMERA_SIZE					0x1104
#define CFG_CAMERA_BIT_WIDTH			0x1105
#define CFG_CAMERA_FORMAT				0x1106
#define CFG_CAMERA_I2C_MODE				0x1107
#define CFG_CAMERA_I2C_ADDR				0x1108
#define CFG_CAMERA_TRANS_LVL			0x1109
#define CFG_CAMERA_G_GAIN				0x110A
#define CFG_CAMERA_B_GAIN				0x110B
#define CFG_CAMERA_R_GAIN				0x110C
#define CFG_CAMERA_G2_GAIN				0x110D
#define CFG_CAMERA_Y_GAIN				0x110E
#define CFG_CAMERA_U_GAIN				0x110F
#define CFG_CAMERA_V_GAIN				0x1110
#define CFG_CAMERA_GL_GAIN				0x1111
#define CFG_CAMERA_OK					0x11FF

#define CFG_BOARD_ERROR					0x1200
#define CFG_BOARD_VR_WRITE				0x1201
#define CFG_BOARD_VR_READ				0x1202
#define CFG_BOARD_OK					0x12FF

#define CFG_REGISTER_ERROR				0x1300
#define CFG_REGISTER_WRITE				0x1301
#define CFG_REGISTER_DELAY				0x1302
#define CFG_REGISTER_BITSET				0x1303
#define CFG_REGISTER_BITCLR				0x1304
#define CFG_REGISTER_WRITE_8_8			0x1305
#define CFG_REGISTER_WRITE_8_16			0x1306
#define CFG_REGISTER_WRITE_16_8			0x1307
#define CFG_REGISTER_WRITE_16_16		0x1308
#define CFG_REGISTER_OK					0x13FF

#define CFG_NONE_CONTENT				0
#define CFG_OK							0xFF

//ARDUCAMCFG_API Uint32 ArduCamCfg_LoadCameraConfig( ArduCamCfgHandle &useHandle, CStdioFile* pcsfile );
ARDUCAMCFG_API Uint32 ArduCamCfg_LoadCameraConfig( ArduCamCfgHandle &useHandle, CString csFilePath );

ARDUCAMCFG_API Uint32 ArduCamCfg_SetCameraConfig( ArduCamCfgHandle useHandle, Uint32 u32ConfigType, Uint32* pu32ConfigData );
ARDUCAMCFG_API Uint32 ArduCamCfg_SetBoardConfig( ArduCamCfgHandle useHandle, ArduCamHandle cUsbCameraHd );
ARDUCAMCFG_API Uint32 ArduCamCfg_SetRegisterConfig( ArduCamCfgHandle useHandle, ArduCamHandle cUsbCameraHd );