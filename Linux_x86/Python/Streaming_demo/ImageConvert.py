import numpy as np
import ArducamSDK
import cv2

global COLOR_BayerGB2BGR,COLOR_BayerRG2BGR,COLOR_BayerGR2BGR,COLOR_BayerBG2BGR

COLOR_BayerBG2BGR = 46
COLOR_BayerGB2BGR = 47
COLOR_BayerRG2BGR = 48
COLOR_BayerGR2BGR = 49

def JPGToMat(data,datasize):
    image = np.frombuffer(data,np.uint8,count = datasize)
    return cv2.imdecode(image,cv2.IMREAD_ANYCOLOR)

def YUVToMat(data,Width,Height):
    image = np.frombuffer(data, np.uint8).reshape( Height,Width , 2 )
    return cv2.cvtColor(image,cv2.COLOR_YUV2BGR_YUYV)
    
def RGB565ToMat(data,Width,Height):
    arr = np.frombuffer(data,dtype=np.uint16).astype(np.uint32)
    arr = ((arr & 0xFF00) >> 8) + ((arr & 0x00FF) << 8)
    arr = 0xFF000000 + ((arr & 0xF800) << 8) + ((arr & 0x07E0) << 5) + ((arr & 0x001F) << 3)

    arr.dtype = np.uint8
    iamge = arr.reshape(Height,Width,4)
    return cv2.flip(iamge,0)

def dBytesToMat(data,bitWidth,Width,Height):
    arr = np.frombuffer(data,dtype=np.uint16)
    arr = (arr >> (bitWidth - 8))
    arr = arr.astype(np.uint8)
    image = arr.reshape(Height,Width,1)
    return image
def separationImage(data,Width,Height):
    arr = np.frombuffer(data,dtype=np.uint16)
    arr1 = arr >> 8
    arr1 = arr1.astype(np.uint8)
    arr2 = arr.astype(np.uint8)

    image1 = arr1.reshape(Height,Width,1)
    image2 = arr2.reshape(Height,Width,1)

    image = np.concatenate([image1, image2], axis=1)
    return image

def convert_color(image,color_mode):
    if color_mode == 0:
        image = cv2.cvtColor(image,COLOR_BayerRG2BGR)
    if color_mode == 1:
        image = cv2.cvtColor(image,COLOR_BayerGR2BGR)
    if color_mode == 2:
        image = cv2.cvtColor(image,COLOR_BayerGB2BGR)
    if color_mode == 3:
        image = cv2.cvtColor(image,COLOR_BayerBG2BGR)
    if color_mode < 0 and color_mode > 3:
        image = cv2.cvtColor(image,COLOR_BayerGB2BGR)
    return image
def convert_image(data,cfg,color_mode):
    Width = cfg["u32Width"]
    Height = cfg["u32Height"]
    bitWidth = cfg["u8PixelBits"]
    datasize = cfg["u32Size"]
    global COLOR_BayerGB2BGR,COLOR_BayerRG2BGR,COLOR_BayerGR2BGR,COLOR_BayerBG2BGR
    image = None
    emImageFmtMode = cfg['emImageFmtMode']
    if emImageFmtMode == ArducamSDK.FORMAT_MODE_JPG:
        image = JPGToMat(data,datasize)
    if emImageFmtMode == ArducamSDK.FORMAT_MODE_YUV:
        image = YUVToMat(data,Width,Height)
    if emImageFmtMode == ArducamSDK.FORMAT_MODE_RGB:
        image = RGB565ToMat(data,Width,Height)
    if emImageFmtMode == ArducamSDK.FORMAT_MODE_MON:
        if cfg["u8PixelBytes"] == 2:
            image = dBytesToMat(data,bitWidth,Width,Height)
        else:
            image = np.frombuffer(data, np.uint8).reshape( Height,Width , 1 )
    if emImageFmtMode == ArducamSDK.FORMAT_MODE_RAW:
        if cfg["u8PixelBytes"] == 2:
            image = dBytesToMat(data,bitWidth,Width,Height)
        else:
            image = np.frombuffer(data, np.uint8).reshape( Height,Width , 1 )
        image = convert_color(image,color_mode)
    if emImageFmtMode == ArducamSDK.FORMAT_MODE_RAW_D:
        image = separationImage(data,Width,Height)
        image = convert_color(image,color_mode)
        pass
    if emImageFmtMode == ArducamSDK.FORMAT_MODE_MON_D:
        image = separationImage(data,Width,Height)
        pass
    return image
