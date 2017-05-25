#pragma once

#include "stdafx.h"

#ifdef	 USBCAMERALIB_EXPORTS
#define  ARDUCAM_API extern "C" _declspec(dllexport)
#else
#define  ARDUCAM_API extern "C" _declspec(dllimport)
#endif

typedef  unsigned int*		ArduCamHandle;


/* -- USB相机类型代码 -- */
//#define CAMERA_MT9P031							0x4D091031

/* -- USB相机驱动错误代码 -- */

#define	USB_CAMERA_NO_ERROR						0x00

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

#define	USB_CAMERA_READ_EMPTY_ERROR				0xFF30
#define	USB_CAMERA_DEL_EMPTY_ERROR				0xFF31

#define	USB_CAMERA_SIZE_EXCEED_ERROR			0xFF51


#define ARDUCAM_VID								0x52CB


typedef enum  {
	I2C_MODE_8_8   = 0,
	I2C_MODE_8_16  = 1,
	I2C_MODE_16_8  = 2,
	I2C_MODE_16_16 = 3
}i2c_mode;


typedef struct  
{
	unsigned long	u32CameraType;

	unsigned long	u32Width;
	unsigned long	u32Height;

	unsigned char	u8PixelBytes;
	unsigned char	u8PixelBits;

	i2c_mode		emI2cMode;
	unsigned long	u32SensorShipAddr;

	unsigned short	u16Vid;								// Micron Imaging Vendor ID for USB

	unsigned long	u32UsbVersion;						// USB模式
}ArduCamCfg;


/* -- 陀螺仪参数 -- */
typedef struct  
{
	Uint32		u32TimeStamp;
	Uint16		u16Index;
	Uint16		u16AccX;
	Uint16		u16AccY;
	Uint16		u16AccZ;
	Uint16		u16GyroX;
	Uint16		u16GyroY;
	Uint16		u16GyroZ;
	Uint16		u16Temp;
}ArduIMUData;


ARDUCAM_API unsigned int ArduCam_autoopen( ArduCamHandle &useHandle, ArduCamCfg* useCfg );

ARDUCAM_API unsigned int ArduCam_scan( unsigned long* pUsbIdxArray, unsigned short useVID );
ARDUCAM_API unsigned int ArduCam_open( ArduCamHandle &useHandle, ArduCamCfg* useCfg, unsigned long usbIdx );
ARDUCAM_API unsigned int ArduCam_close( ArduCamHandle useHandle );

ARDUCAM_API unsigned int ArduCam_beginCaptureImage( ArduCamHandle useHandle );
ARDUCAM_API unsigned int ArduCam_captureImage( ArduCamHandle useHandle );
ARDUCAM_API unsigned int ArduCam_endCaptureImage( ArduCamHandle useHandle );

ARDUCAM_API unsigned int ArduCam_captureIMU( ArduCamHandle useHandle );

ARDUCAM_API unsigned int ArduCam_availiableImage( ArduCamHandle useHandle );
ARDUCAM_API unsigned int ArduCam_availiableIMU( ArduCamHandle useHandle );
ARDUCAM_API unsigned int ArduCam_readImage( ArduCamHandle useHandle, unsigned char* &pu8FrameData );	
ARDUCAM_API unsigned int ArduCam_readIMU( ArduCamHandle useHandle, unsigned short u16Index, ArduIMUData* pstIMUData );
ARDUCAM_API unsigned int ArduCam_readImageAndIMU( ArduCamHandle useHandle, unsigned char* &pu8FrameData, ArduIMUData* &pstIMUData );

ARDUCAM_API unsigned int ArduCam_del( ArduCamHandle useHandle );							
ARDUCAM_API unsigned int ArduCam_flush( ArduCamHandle useHandle );

ARDUCAM_API unsigned int ArduCam_shot( ArduCamHandle useHandle );

ARDUCAM_API unsigned int ArduCam_writeSensorReg( ArduCamHandle useHandle, unsigned long regAddr, unsigned long val );
ARDUCAM_API unsigned int ArduCam_readSensorReg( ArduCamHandle useHandle, unsigned long regAddr, unsigned long* pval );

ARDUCAM_API unsigned int ArduCam_getSensorCfg( ArduCamHandle useHandle, ArduCamCfg* useCfg );

ARDUCAM_API unsigned int ArduCam_writeReg_8_8( ArduCamHandle useHandle, unsigned long shipAddr, unsigned long regAddr, unsigned long val );
ARDUCAM_API unsigned int ArduCam_readReg_8_8( ArduCamHandle useHandle, unsigned long shipAddr, unsigned long regAddr, unsigned long* pval );
ARDUCAM_API unsigned int ArduCam_writeReg_8_16( ArduCamHandle useHandle, unsigned long shipAddr, unsigned long regAddr, unsigned long val );
ARDUCAM_API unsigned int ArduCam_readReg_8_16( ArduCamHandle useHandle, unsigned long shipAddr, unsigned long regAddr, unsigned long* pval );
ARDUCAM_API unsigned int ArduCam_writeReg_16_8( ArduCamHandle useHandle, unsigned long shipAddr, unsigned long regAddr, unsigned long val );
ARDUCAM_API unsigned int ArduCam_readReg_16_8( ArduCamHandle useHandle, unsigned long shipAddr, unsigned long regAddr, unsigned long* pval );
ARDUCAM_API unsigned int ArduCam_writeReg_16_16( ArduCamHandle useHandle, unsigned long shipAddr, unsigned long regAddr, unsigned long val );
ARDUCAM_API unsigned int ArduCam_readReg_16_16( ArduCamHandle useHandle, unsigned long shipAddr, unsigned long regAddr, unsigned long* pval );

ARDUCAM_API void ArduCam_enableForceRead( ArduCamHandle useHandle );
ARDUCAM_API void ArduCam_disableForceRead( ArduCamHandle useHandle );





