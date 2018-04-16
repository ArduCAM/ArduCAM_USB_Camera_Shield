#ifndef ARDUCAMLIB_H
#define ARDUCAMLIB_H

extern "C"{

    typedef     int                                BOOL;
    typedef     char                               Int8;
    typedef     short                              Int16;
    typedef     int                                Int32;
    typedef		unsigned char                      Uint8;
    typedef		unsigned short                     Uint16;
    typedef		unsigned int                       Uint32;
    typedef     unsigned int*                      ArduCamHandle;

    #define RAW_RG 0
    #define RAW_GR 1
    #define RAW_GB 2
    #define RAW_BG 3
    /* -- USB相机驱动错误代码 -- */

    #define	USB_CAMERA_NO_ERROR						0x00
    #define	USB_CAMERA_USB_CREATE_ERROR				0x01
    #define	USB_CAMERA_USB_SET_CONTEXT_ERROR		0x02
    #define	USB_CAMERA_VR_COMMAND_ERROR				0x03
    #define	USB_CAMERA_USB_VERSION_ERROR			0x04
    #define	USB_CAMERA_BUFFER_ERROR					0x05
    #define	USB_CAMERA_HANDLE_ERROR					0x06
    #define	USB_CAMERA_HANDLE_CREATE_ERROR			0x07


    #define USB_CAMERA_I2C_BIT_ERROR				0x0B
    #define USB_CAMERA_I2C_NACK_ERROR				0x0C
    #define USB_CAMERA_I2C_TIMEOUT					0x0D

    #define USB_CAMERA_USB_TASK_ERROR				0x20
    #define	USB_CAMERA_DATA_OVERFLOW_ERROR			0x21
    #define	USB_CAMERA_DATA_LACK_ERROR				0x22
    #define	USB_CAMERA_FIFO_FULL_ERROR				0x23
    #define	USB_CAMERA_DATA_LEN_ERROR				0x24

    #define	USB_CAMERA_READ_EMPTY_ERROR				0x30
    #define	USB_CAMERA_DEL_EMPTY_ERROR				0x31

    #define	USB_CAMERA_SIZE_EXCEED_ERROR			0x51

    #define  HANDLE_PROXY_MAX_SUM                   16

    typedef enum {
        I2C_MODE_8_8   = 0,
        I2C_MODE_8_16  = 1,
        I2C_MODE_16_8  = 2,
        I2C_MODE_16_16 = 3
    }i2c_mode;

    typedef struct
    {
        Uint32	u32CameraType;
        Uint32	u32Width;
        Uint32	u32Height;
        Uint32	u32UsbVersion;
        Uint32	u8PixelBytes;
        Uint32  u16Vid;

        Uint32  u32SensorShipAddr;
        Uint8   u8PixelBits;
        i2c_mode emI2cMode;

    }ArduCamCfg;

    unsigned int ArduCam_autoopen( ArduCamHandle &useHandle, ArduCamCfg *useCfg );
    unsigned int ArduCam_close( ArduCamHandle useHandle);

    unsigned int ArduCam_capture( ArduCamHandle useHandle);
    unsigned int ArduCam_read( ArduCamHandle useHandle,Uint8* &pu8FrameData );
    unsigned int ArduCam_del( ArduCamHandle useHandle);
    unsigned int ArduCam_flush( ArduCamHandle useHandle);
    unsigned int ArduCam_available( ArduCamHandle useHandle);

    unsigned int ArduCam_writeSensorReg( ArduCamHandle useHandle,Uint32 regAddr, Uint32 val );
    unsigned int ArduCam_readSensorReg( ArduCamHandle useHandle,Uint32 regAddr, Uint32* pval );
    unsigned int ArduCam_getSensorCfg( ArduCamHandle useHandle,ArduCamCfg* useCfg );

    unsigned int ArduCam_scan( Uint32* pUsbIdxArray, Uint16 useVID );
    unsigned int ArduCam_open( ArduCamHandle &useHandle, ArduCamCfg* useCfg, Uint32 usbIdx );
    unsigned int ArduCam_beginCapture(ArduCamHandle useHandle);
    unsigned int ArduCam_endCapture( ArduCamHandle useHandle );
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
}

#endif // ARDUCAMLIB_H
