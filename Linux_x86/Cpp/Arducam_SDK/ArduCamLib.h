#ifndef ARDUCAMLIB_H
#define ARDUCAMLIB_H

extern "C"{

    typedef     int                                BOOL;
    typedef     char                               Int8;
    typedef     short                              Int16;
    typedef     int                                Int32;
    typedef	unsigned char                      Uint8;
    typedef	unsigned short                     Uint16;
    typedef	unsigned int                       Uint32;
    typedef     unsigned int*                      ArduCamHandle;

    #define RAW_RG 0
    #define RAW_GR 1
    #define RAW_GB 2
    #define RAW_BG 3
    /* -- USB相机驱动错误代码 -- */
    #define USB_CAMERA_NO_ERROR				0x00
    #define USB_CAMERA_USB_CREATE_ERROR			0x01
    #define USB_CAMERA_USB_SET_CONTEXT_ERROR		0x02
    #define USB_CAMERA_VR_COMMAND_ERROR			0x03
    #define USB_CAMERA_USB_VERSION_ERROR		0x04
    #define USB_CAMERA_BUFFER_ERROR			0x05
    #define USB_CAMERA_NOT_FOUND_DEVICE_ERROR		0x06

    #define USB_CAMERA_I2C_BIT_ERROR			0x0B
    #define USB_CAMERA_I2C_NACK_ERROR			0x0C
    #define USB_CAMERA_I2C_TIMEOUT			0x0D

    #define USB_CAMERA_USB_TASK_ERROR			0x20
    #define USB_CAMERA_DATA_OVERFLOW_ERROR		0x21
    #define USB_CAMERA_DATA_LACK_ERROR			0x22
    #define USB_CAMERA_FIFO_FULL_ERROR			0x23
    #define USB_CAMERA_DATA_LEN_ERROR			0x24
    #define USB_CAMERA_FRAME_INDEX_ERROR		0x25

    #define USB_CAMERA_READ_EMPTY_ERROR			0x30
    #define USB_CAMERA_DEL_EMPTY_ERROR			0x31

    #define USB_CAMERA_SIZE_EXCEED_ERROR		0x51

    #define USB_USERDATA_ADDR_ERROR			0xFF61
    #define USB_USERDATA_LEN_ERROR			0xFF62

    /* -- ArduCamCfg->u32UsbVersion value -- */
    #define USB_1					1      //USB 1.0
    #define USB_2					2      //USB 2.0
    #define USB_3					3      //USB 3.0
    #define USB_3_2					4      //USB3.0 mode and USB2.0 interface

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
        Uint32	u32Width;
        Uint32	u32Height;
        Uint32	u32UsbVersion;
        Uint32	u8PixelBytes;
        Uint32  u16Vid;
        Uint32  u32Size;
        Uint32  u32SensorShipAddr;
        Uint8   u8PixelBits;
        i2c_mode emI2cMode;
        format_mode emImageFmtMode;
    }ArduCamCfg;

    /* -- camera output data -- */
    typedef struct  
    {
        ArduCamCfg	stImagePara;
        Uint8*		pu8ImageData;
    }ArduCamOutData;

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

    unsigned int ArduCam_scan( Uint32* pUsbIdxArray, Uint16 useVID );
    unsigned int ArduCam_open( ArduCamHandle &useHandle, ArduCamCfg* useCfg, Uint32 usbIdx );
    unsigned int ArduCam_beginCaptureImage(ArduCamHandle useHandle);
    unsigned int ArduCam_endCaptureImage( ArduCamHandle useHandle );
    unsigned int ArduCam_shot( ArduCamHandle useHandle );
    unsigned int ArduCam_writeReg_8_8( ArduCamHandle useHandle, Uint32 shipAddr, Uint32 regAddr, Uint32 val );
    unsigned int ArduCam_readReg_8_8( ArduCamHandle useHandle, Uint32 shipAddr, Uint32 regAddr, Uint32* pval );
    unsigned int ArduCam_writeReg_8_16( ArduCamHandle useHandle, Uint32 shipAddr, Uint32 regAddr, Uint32 val );
    unsigned int ArduCam_readReg_8_16( ArduCamHandle useHandle, Uint32 shipAddr, Uint32 regAddr, Uint32* pval );
    unsigned int ArduCam_writeReg_16_8( ArduCamHandle useHandle, Uint32 shipAddr, Uint32 regAddr, Uint32 val );
    unsigned int ArduCam_readReg_16_8( ArduCamHandle useHandle, Uint32 shipAddr, Uint32 regAddr, Uint32* pval );
    unsigned int ArduCam_writeReg_16_16( ArduCamHandle useHandle, Uint32 shipAddr, Uint32 regAddr, Uint32 val );
    unsigned int ArduCam_readReg_16_16( ArduCamHandle useHandle, Uint32 shipAddr, Uint32 regAddr, Uint32* pval );
    unsigned int ArduCam_VRCMD(ArduCamHandle useHandle, Uint8 u8Command, Uint8 u8Direction, Uint16 u16Vvalue, Uint16 u16Index, Uint32 u32BufSize, Uint8 *pu8Buf, Uint32 *pu32DataNum );
    void ArduCam_enableForceRead( ArduCamHandle useHandle );
    void ArduCam_disableForceRead( ArduCamHandle useHandle );
    unsigned int ArduCam_setboardConfig( ArduCamHandle useHandle, Uint8 u8Command, Uint16 u16Value, Uint16 u16Index, Uint32 u32BufSize, Uint8 *pu8Buf );
    unsigned int ArduCam_getboardConfig( ArduCamHandle useHandle, Uint8 u8Command, Uint16 u16Value, Uint16 u16Index, Uint32 u32BufSize, Uint8 *pu8Buf );
    unsigned int ArduCam_readUserData( ArduCamHandle useHandle, Uint16 u16Addr, Uint8 u8Len, Uint8* pu8Data );
    unsigned int ArduCam_writeUserData( ArduCamHandle useHandle, Uint16 u16Addr, Uint8 u8Len, Uint8* pu8Data );
}

#endif // ARDUCAMLIB_H
