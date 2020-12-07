#pragma once
#pragma pack(8)

#include "stdafx.h"

/* --------------- Type Standardize --------------- */
typedef		float			f32;		// 32位浮点型
typedef		double			f64;		// 16位浮点型
typedef		long			Int32;		// 32位有符号整型
typedef		unsigned long	Uint32;		// 32位无符号整型


typedef		short			Int16;		// 16位有符号整型
typedef		char			Int8;		//  8位有符号整型
typedef		unsigned short	Uint16;		// 16位无符号整型
typedef		unsigned char	Uint8;		//  8位无符号整型

#if defined(_WIN64)
typedef __int64           Int;
#elif defined(__GNUC__)
typedef __attribute__((mode (__pointer__))) int mi_intptr;
#elif defined(_MSC_VER) && _MSC_VER >= 0x1300
typedef __w64 int         Int;
#else
typedef int               Int;
#endif

typedef  Uint32* ArduCamHandle;
/* --------------- Type Standardize --------------- */

#ifdef	 USBCAMERALIB_EXPORTS
#define  ARDUCAM_API extern "C" _declspec(dllexport)
#else
#define  ARDUCAM_API extern "C" _declspec(dllimport)
#endif

#define	FORCE_DISPLAY						// 强制显示

/* --------------- Error Code --------------- */
#define	USB_CAMERA_NO_ERROR						0x0000

#define	USB_CAMERA_USB_CREATE_ERROR				0xFF01
#define	USB_CAMERA_USB_SET_CONTEXT_ERROR		0xFF02
#define	USB_CAMERA_VR_COMMAND_ERROR				0xFF03
#define	USB_CAMERA_USB_VERSION_ERROR			0xFF04
#define	USB_CAMERA_BUFFER_ERROR					0xFF05

#define USB_CAMERA_I2C_BIT_ERROR				0xFF0B
#define USB_CAMERA_I2C_NACK_ERROR				0xFF0C
#define USB_CAMERA_I2C_TIMEOUT					0xFF0D

#define USB_CAMERA_USB_TASK_ERROR				0xFF20
#define	USB_CAMERA_DATA_OVERFLOW_ERROR			0xFF21
#define	USB_CAMERA_DATA_LACK_ERROR				0xFF22
#define	USB_CAMERA_FIFO_FULL_ERROR				0xFF23
#define	USB_CAMERA_DATA_LEN_ERROR				0xFF24
#define	USB_CAMERA_FRAME_INDEX_ERROR			0xFF25
#define USB_CAMERA_USB_TIMEOUT_ERROR			0xFF26

#define	USB_CAMERA_READ_EMPTY_ERROR				0xFF30
#define	USB_CAMERA_DEL_EMPTY_ERROR				0xFF31

#define	USB_CAMERA_SIZE_EXCEED_ERROR			0xFF51

#define	USB_USERDATA_ADDR_ERROR					0xFF61
#define	USB_USERDATA_LEN_ERROR					0xFF62
/* --------------- Error Code --------------- */

#define MAX_IMAGE_WIDTH							16384
#define	MAX_IMAGE_HEIGHT						8192
#define	MAX_IMAGE_SIZE							( MAX_IMAGE_WIDTH * MAX_IMAGE_HEIGHT )

#define USB_1									1      //USB 1.0 (EZUSB) is supported
#define USB_2									2      //USB 2.0 (FX2) is supported
#define USB_3									3      //USB 3.0 (FX3) is supported

/* -- I2C mode -- */
typedef enum  {
	I2C_MODE_8_8   = 0,
	I2C_MODE_8_16  = 1,
	I2C_MODE_16_8  = 2,
	I2C_MODE_16_16 = 3
}i2c_mode;

/* -- Image format mode -- */
typedef enum  {
	FORMAT_MODE_RAW = 0,
	FORMAT_MODE_RGB = 1,
	FORMAT_MODE_YUV = 2,
	FORMAT_MODE_JPG = 3,
	FORMAT_MODE_MON = 4,
	FORMAT_MODE_RAW_D = 5,
	FORMAT_MODE_MON_D = 6,
	FORMAT_MODE_TOF = 7,
}format_mode;

/* -- USB driver parameters -- */
typedef struct
{
	void*        pvdUSBDevice;
	Uint8        u8DeviceNum;
	Int			 iHWnd;
} USB2DriverInfo;


typedef struct
{
	Uint8   u8UsbIndex;
	Uint8	u8SerialNum[16];
} ArduCamIndexinfo;


/* -- camera parameters -- */
typedef struct  
{
	Uint32	u32CameraType;				// Camera Type
	Uint16  u16Vid;                     // Vendor ID for USB
	Uint32	u32Width;					// Image Width
	Uint32	u32Height;					// Image Height
	Uint8	u8PixelBytes;
	Uint8   u8PixelBits;
	Uint32  u32I2cAddr;
	Uint32  u32Size;
	Uint8   usbType;
	i2c_mode emI2cMode;
	format_mode emImageFmtMode;			// image format mode 
	Uint32	u32TransLvl;
}ArduCamCfg;


/* -- camera output data -- */
typedef struct  
{
	ArduCamCfg	stImagePara;
	__int64		u64Time;
	Uint8*		pu8ImageData;
}ArduCamOutData;


ARDUCAM_API Uint32 ArduCam_autoopen( ArduCamHandle &useHandle, ArduCamCfg* useCfg );

ARDUCAM_API Uint32 ArduCam_scan( ArduCamIndexinfo* pstUsbIdxArray );
ARDUCAM_API Uint32 ArduCam_open( ArduCamHandle &useHandle, ArduCamCfg* useCfg, Uint32 usbIdx );
ARDUCAM_API Uint32 ArduCam_close( ArduCamHandle useHandle );

ARDUCAM_API Uint32 ArduCam_beginCaptureImage( ArduCamHandle useHandle );
ARDUCAM_API Uint32 ArduCam_captureImage( ArduCamHandle useHandle );
ARDUCAM_API Uint32 ArduCam_endCaptureImage( ArduCamHandle useHandle );

ARDUCAM_API Uint32 ArduCam_availableImage( ArduCamHandle useHandle );
ARDUCAM_API Uint32 ArduCam_readImage( ArduCamHandle useHandle, ArduCamOutData* &pstFrameData );	

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
ARDUCAM_API Uint32 ArduCam_getboardConfig( ArduCamHandle useHandle, Uint8 u8Command, Uint16 u16Value, Uint16 u16Index, Uint32 u32BufSize, Uint8 *pu8Buf );

ARDUCAM_API Uint32 ArduCam_readUserData( ArduCamHandle useHandle, Uint16 u16Addr, Uint8 u8Len, Uint8* pu8Data );
ARDUCAM_API Uint32 ArduCam_writeUserData( ArduCamHandle useHandle, Uint16 u16Addr, Uint8 u8Len, Uint8* pu8Data );

ARDUCAM_API void ArduCam_enableForceRead( ArduCamHandle useHandle );
ARDUCAM_API void ArduCam_disableForceRead( ArduCamHandle useHandle );






