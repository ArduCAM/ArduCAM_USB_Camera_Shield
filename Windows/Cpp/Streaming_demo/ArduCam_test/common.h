#ifndef _COMMON_H_
#define _COMMON_H_

#pragma pack(8)

//#define Linux_ARCH
#define Win_ARCH

#define FORCE_DISPLAY					// 强制显示

#ifdef Linux_ARCH
	#include "unistd.h"
	#include "stdio.h"
#endif

#ifdef Win_ARCH
	#include "stdafx.h"
#endif

#ifdef Linux_ARCH
#define Sleep(t) usleep(1000*t)
#define max(a,b)    (((a) > (b)) ? (a) : (b))
#define min(a,b)    (((a) < (b)) ? (a) : (b))
#endif

#include <stdint.h>
/* -------------------- Type Standardize -------------------- */
typedef		float			f32;		// 32位浮点型
typedef		double			f64;		// 16位浮点型

#ifdef Linux_ARCH
typedef		long			Int64;		// 64位有符号整型
typedef		int				Int32;		// 32位有符号整型
typedef		unsigned long	Uint64;		// 64位无符号整型
typedef		unsigned int	Uint32;		// 32位无符号整型
#endif

#ifdef Win_ARCH
typedef		long			Int32;		// 32位有符号整型
typedef		unsigned long	Uint32;		// 32位无符号整型
#endif

typedef		short			Int16;		// 16位有符号整型
typedef		char			Int8;		//  8位有符号整型
typedef		unsigned short	Uint16;		// 16位无符号整型
typedef		unsigned char	Uint8;		//  8位无符号整型

typedef  Uint32* ArduCamHandle;
typedef  Uint32* ArduCamCfgHandle;

#if defined(_WIN64)
typedef __int64           Int;
#elif defined(__GNUC__)
typedef __attribute__((mode (__pointer__))) int mi_intptr;
#elif defined(_MSC_VER) && _MSC_VER >= 0x1300
typedef __w64 int         Int;
#else
typedef int               Int;
#endif
/* -------------------- Type Standardize -------------------- */

#pragma pack(8)

#define RAW_RG                              0
#define RAW_GR                              1
#define RAW_GB                              2
#define RAW_BG                              3

/* --------------- USB相机驱动错误代码 --------------- */
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

#define	USB_IMU_READ_ERROR						0xFF71
#define	USB_IMU_CAPT_ERROR						0xFF72
/* --------------- USB相机驱动错误代码 --------------- */


#define MAX_IMAGE_WIDTH							4608
#define	MAX_IMAGE_HEIGHT						3288
#define	MAX_IMAGE_SIZE							( MAX_IMAGE_WIDTH * MAX_IMAGE_HEIGHT )

#define FRAME_FIFO_NUM							6
#define USB_TASK_MAX_NUM						64

#define MAX_PACKET_SIZE							512				// MAX Packet size for all devices
#define MI_MAX_BULK_TRANSFER_SIZE				(1024*64)		// (640*480+512)//(1024*1024)
#define MARKER_PACKET_SIZE						512				
#define MARKER_SIZE								10		

#define USB_1									1      //USB 1.0 (EZUSB) is supported
#define USB_2									2      //USB 2.0 (FX2) is supported
#define USB_3									3      //USB 3.0 (FX3) is supported
#define USB_3_2									4      //USB 3.0 device USB2.0 interface

#define EXTERNAL_TRIGGER_MODE					0x01
#define CONTINUOUS_MODE							0x02
#define USB_BOARD_FW_VERSION_NOT_SUPPORT_ERROR  0xFF71

#define CY_USB									1      //This refers to the original cypress driver w/ source
#define CY_USB2									2      //This refers to the 2nd cypress driver which used CyAPI library

//returns a padded size which is evenly divisible by packet_size
#define ALIGN(bytes,packet_size)    ((int)((bytes+packet_size) / packet_size) * packet_size)


/* --------------- Vendor TYPE --------------- */
#define VR_HOST_TO_DEVICE			0x00
#define VR_DEVICE_TO_HOST			0x01

#define VR_I2C_WRITE_8_8            0xD7	// Write I2C register w/ 8bit address 8bit data
#define VR_I2C_READ_8_8             0xD6	// Read  I2C register w/ 8bit address 8 bit data

#define VR_I2C_WRITE_8_16			0xD5	// Write I2C register w/ 8bit address 16bit data
#define VR_I2C_READ_8_16			0xD4	// Read  I2C register w/ 8bit address 16bit data

#define VR_I2C_WRITE_16_16          0xE1	// Write I2C register w/ 16bit address 16bit data
#define VR_I2C_READ_16_16           0xE2	// Read  I2C register w/ 16bit address 16bit data

#define VR_I2C_WRITE_16_8           0xE0	// Write I2C register w/ 16bit address 8bit data
#define VR_I2C_READ_16_8            0xDF	// Read  I2C register w/ 16bit address 8bit data 

#define VR_I2C_WRITE_USERDATA		0xF8	// Write user region data
#define VR_I2C_READ_USERDATA		0xF7	// Read  user region data
/* --------------- Vendor TYPE --------------- */


/* --------------- Config TYPE --------------- */
#define CFG_NONE_CONTENT				0

#define CFG_TITLE_ERROR					0x10
#define CFG_TITLE_CAMERA				0x11
#define CFG_TITLE_BOARD					0x12
#define CFG_TITLE_REGISTER				0x13

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
/* --------------- Config TYPE --------------- */


/* --------------- Record --------------- */
#define RECORD_NULL					0
#define RECORD_DATA					1
#define RECORD_IMAGE				2
#define RECORD_VIDEO				3
/* --------------- Record --------------- */


/* -- Double Image display mode -- */
#define MODE_HORIZONTAL			0
#define MODE_VERTICAL			1


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


typedef struct  
{
	Uint32		u32Page;
	Uint32		u32Addr;
	Uint32		u32MinValue;
	Uint32		u32MaxValue;
}GainSet;

#if !defined(__ARDUCAM_STRUCT_CONTROL__)
#define __ARDUCAM_STRUCT_CONTROL__
typedef struct {
    int64_t min;
    int64_t max;
    int32_t step;
    int64_t def;
    uint32_t flags;
    char name[128];
    char func[128];
    char *code;
} Control;
#endif


//extern UINT _FrameCaptureThread( LPVOID lParam );						// 图像采集线程
//extern UINT _FrameReadThread( LPVOID lParam );							// 图像读取显示线程

#endif