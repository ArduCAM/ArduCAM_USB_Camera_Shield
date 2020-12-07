#ifndef ARDUCAMLIB_H
#define ARDUCAMLIB_H

extern "C"{

    typedef int                                 BOOL;
    typedef char                                Int8;
    typedef short                               Int16;
    typedef int                                 Int32;
    typedef long long                           Int64;
    typedef	unsigned char                       Uint8;
    typedef	unsigned short                      Uint16;
    typedef	unsigned int                        Uint32;
    typedef unsigned long long                  UInt64;
    typedef void*                               ArduCamHandle;

    #define RAW_RG                              0
    #define RAW_GR                              1
    #define RAW_GB                              2
    #define RAW_BG                              3



    /* -- ArduCamCfg->u32UsbVersion value -- */
    #define USB_1                               1      //USB 1.0
    #define USB_2                               2      //USB 2.0
    #define USB_3                               3      //USB 3.0
    #define USB_3_2                             4      //USB3.0 mode and USB2.0 interface

    #define EXTERNAL_TRIGGER_MODE               0x01
    #define CONTINUOUS_MODE                     0x02
    
    #define	USB_CAMERA_NO_ERROR						0x0000
    #define	USB_CAMERA_USB_CREATE_ERROR				0xFF01
    #define	USB_CAMERA_USB_SET_CONTEXT_ERROR		0xFF02
    #define	USB_CAMERA_VR_COMMAND_ERROR				0xFF03
    #define	USB_CAMERA_USB_VERSION_ERROR			0xFF04
    #define	USB_CAMERA_BUFFER_ERROR					0xFF05
    #define	USB_CAMERA_NOT_FOUND_DEVICE_ERROR		0xFF06

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

    #define USB_BOARD_FW_VERSION_NOT_SUPPORT_ERROR  0xFF71
    
    
    typedef enum {
        I2C_MODE_8_8   = 0,
        I2C_MODE_8_16  = 1,
        I2C_MODE_16_8  = 2,
        I2C_MODE_16_16 = 3
    }i2c_mode;
    typedef enum{
        FORMAT_MODE_RAW = 0,
        FORMAT_MODE_RGB = 1,
        FORMAT_MODE_YUV = 2,
        FORMAT_MODE_JPG = 3,
        FORMAT_MODE_MON = 4,
        FORMAT_MODE_RAW_D = 5,
        FORMAT_MODE_MON_D = 6,
    }format_mode;

    typedef struct
    {
        Uint32	u32CameraType;
        Uint16  u16Vid;                              //Micron Imaging Vendor ID for USB
        Uint32	u32Width;
        Uint32	u32Height;
        Uint8	u8PixelBytes;
        Uint8   u8PixelBits;
        Uint32  u32I2cAddr;
        Uint32  u32Size;
        Uint8   usbType; 
        i2c_mode emI2cMode;
        format_mode emImageFmtMode;
        Uint32	u32TransLvl;
    }ArduCamCfg;

    typedef struct
    {
        Uint8   u8UsbIndex;
        Uint8	u8SerialNum[16];
    } ArduCamIndexinfo;
    /* -- camera output data -- */
    typedef struct  
    {
        ArduCamCfg	stImagePara;
        UInt64      u64Time;
        Uint8*		pu8ImageData;
    }ArduCamOutData;

#if !defined(__ARDUCAM_STRUCT_CONTROL__)
#define __ARDUCAM_STRUCT_CONTROL__
#include <stdint.h>
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

    unsigned int ArduCam_autoopen( ArduCamHandle &useHandle, ArduCamCfg *useCfg );
    unsigned int ArduCam_close( ArduCamHandle useHandle);
    void ArduCam_setForceOutput(ArduCamHandle useHandle,bool value);
    unsigned int ArduCam_captureImage( ArduCamHandle useHandle);
    unsigned int ArduCam_readImage( ArduCamHandle useHandle, ArduCamOutData* &pstFrameData );	
    unsigned int ArduCam_del( ArduCamHandle useHandle);
    unsigned int ArduCam_flush( ArduCamHandle useHandle);
    unsigned int ArduCam_availableImage( ArduCamHandle useHandle);

    unsigned int ArduCam_writeSensorReg( ArduCamHandle useHandle,Uint32 regAddr, Uint32 val );
    unsigned int ArduCam_readSensorReg( ArduCamHandle useHandle,Uint32 regAddr, Uint32* pval );
    unsigned int ArduCam_getSensorCfg( ArduCamHandle useHandle,ArduCamCfg* useCfg );
    unsigned int ArduCam_scan(ArduCamIndexinfo *pstUsbIdxArray);
    unsigned int ArduCam_open( ArduCamHandle &useHandle, ArduCamCfg* useCfg, Uint32 usbIdx );
    unsigned int ArduCam_beginCaptureImage(ArduCamHandle useHandle);
    unsigned int ArduCam_endCaptureImage( ArduCamHandle useHandle );
    unsigned int ArduCam_writeReg_8_8( ArduCamHandle useHandle, Uint32 shipAddr, Uint32 regAddr, Uint32 val );
    unsigned int ArduCam_readReg_8_8( ArduCamHandle useHandle, Uint32 shipAddr, Uint32 regAddr, Uint32* pval );
    unsigned int ArduCam_writeReg_8_16( ArduCamHandle useHandle, Uint32 shipAddr, Uint32 regAddr, Uint32 val );
    unsigned int ArduCam_readReg_8_16( ArduCamHandle useHandle, Uint32 shipAddr, Uint32 regAddr, Uint32* pval );
    unsigned int ArduCam_writeReg_16_8( ArduCamHandle useHandle, Uint32 shipAddr, Uint32 regAddr, Uint32 val );
    unsigned int ArduCam_readReg_16_8( ArduCamHandle useHandle, Uint32 shipAddr, Uint32 regAddr, Uint32* pval );
    unsigned int ArduCam_writeReg_16_16( ArduCamHandle useHandle, Uint32 shipAddr, Uint32 regAddr, Uint32 val );
    unsigned int ArduCam_readReg_16_16( ArduCamHandle useHandle, Uint32 shipAddr, Uint32 regAddr, Uint32* pval );
    void ArduCam_enableForceRead( ArduCamHandle useHandle );
    void ArduCam_disableForceRead( ArduCamHandle useHandle );
    unsigned int ArduCam_setboardConfig( ArduCamHandle useHandle, Uint8 u8Command, Uint16 u16Value, Uint16 u16Index, Uint32 u32BufSize, Uint8 *pu8Buf );
    unsigned int ArduCam_getboardConfig( ArduCamHandle useHandle, Uint8 u8Command, Uint16 u16Value, Uint16 u16Index, Uint32 u32BufSize, Uint8 *pu8Buf );
    unsigned int ArduCam_readUserData( ArduCamHandle useHandle, Uint16 u16Addr, Uint8 u8Len, Uint8* pu8Data );
    unsigned int ArduCam_writeUserData( ArduCamHandle useHandle, Uint16 u16Addr, Uint8 u8Len, Uint8* pu8Data );
    unsigned int ArduCam_softTrigger(ArduCamHandle useHandle);
    unsigned int ArduCam_isFrameReady(ArduCamHandle useHandle);
    unsigned int ArduCam_getSingleFrame(ArduCamHandle useHandle, ArduCamOutData* &pstFrameData,int time_out = 1500);
    unsigned int ArduCam_setMode(ArduCamHandle useHandle, int mode);
    int ArduCam_registerCtrls(ArduCamHandle useHandle, Control *controls, Uint32 controls_length);
    int ArduCam_setCtrl(ArduCamHandle useHandle, const char *func_name, Int64 val);
}

#endif // ARDUCAMLIB_H
