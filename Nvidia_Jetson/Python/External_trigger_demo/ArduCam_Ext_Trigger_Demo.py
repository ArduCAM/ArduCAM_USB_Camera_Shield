import sys
import os
import time
import cv2
import threading
import numpy as np
import signal
import json
from ImageConvert import *
import arducam_config_parser
import ArducamSDK


global cfg,running,Width,Heigth,save_flag,color_mode,handles,totalFrames,save_raw
running = True
save_flag = True
save_raw = False
cfg = {}
handles = []
totalFrames = []
global COLOR_BayerGB2BGR,COLOR_BayerRG2BGR,COLOR_BayerGR2BGR,COLOR_BayerBG2BGR

COLOR_BayerBG2BGR = 46
COLOR_BayerGB2BGR = 47
COLOR_BayerRG2BGR = 48
COLOR_BayerGR2BGR = 49


def configBoard(handle, config):
    ArducamSDK.Py_ArduCam_setboardConfig(handle, config.params[0], \
        config.params[1], config.params[2], config.params[3], \
            config.params[4:config.params_length])

pass


def camera_initFromFile(fileName,index):
    global cfg,Width,Height,color_mode,save_raw
    #load config file
    config = arducam_config_parser.LoadConfigFile(fileName)

    camera_parameter = config.camera_param.getdict()
    Width = camera_parameter["WIDTH"]
    Height = camera_parameter["HEIGHT"]

    BitWidth = camera_parameter["BIT_WIDTH"]
    ByteLength = 1
    if BitWidth > 8 and BitWidth <= 16:
        ByteLength = 2
        save_raw = True
    FmtMode = camera_parameter["FORMAT"][0]
    color_mode = camera_parameter["FORMAT"][1]

    I2CMode = camera_parameter["I2C_MODE"]
    I2cAddr = camera_parameter["I2C_ADDR"]
    TransLvl = camera_parameter["TRANS_LVL"]
    cfg = {"u32CameraType":0x00,
            "u32Width":Width,"u32Height":Height,
            "usbType":0,
            "u8PixelBytes":ByteLength,
            "u16Vid":0,
            "u32Size":0,
            "u8PixelBits":BitWidth,
            "u32I2cAddr":I2cAddr,
            "emI2cMode":I2CMode,
            "emImageFmtMode":FmtMode,
            "u32TransLvl":TransLvl }

    # ArducamSDK.
    ret,handle,rtn_cfg = ArducamSDK.Py_ArduCam_open(cfg,index)
    #ret,handle,rtn_cfg = ArducamSDK.Py_ArduCam_autoopen(cfg)
    if ret == 0:
       
        #ArducamSDK.Py_ArduCam_writeReg_8_8(handle,0x46,3,0x00)
        usb_version = rtn_cfg['usbType']
        #print("USB VERSION:",usb_version)
        configs = config.configs
        configs_length = config.configs_length
        for i in range(configs_length):
            type = configs[i].type
            if ((type >> 16) & 0xFF) != 0 and ((type >> 16) & 0xFF) != usb_version:
                continue
            if type & 0xFFFF == arducam_config_parser.CONFIG_TYPE_REG:
                ArducamSDK.Py_ArduCam_writeSensorReg(handle, configs[i].params[0], configs[i].params[1])
            elif type & 0xFFFF == arducam_config_parser.CONFIG_TYPE_DELAY:
                time.sleep(float(configs[i].params[0])/1000)
            elif type & 0xFFFF == arducam_config_parser.CONFIG_TYPE_VRCMD:
                configBoard(handle, configs[i])

        rtn_val,datas = ArducamSDK.Py_ArduCam_readUserData(handle,0x400-16, 16)
        print("Serial: %c%c%c%c-%c%c%c%c-%c%c%c%c"%(datas[0],datas[1],datas[2],datas[3],
                                                    datas[4],datas[5],datas[6],datas[7],
                                                    datas[8],datas[9],datas[10],datas[11]))

        return handle
    else:
        print("open fail,ret_val = ",ret)
        return None

pass
       


def getAndDisplaySingleFrame(handle,index):
    global running,Width,Height,save_flag,cfg,color_mode,totalFrames,save_raw
    global COLOR_BayerGB2BGR,COLOR_BayerRG2BGR,COLOR_BayerGR2BGR,COLOR_BayerBG2BGR
    count = 0
    totalFrame = totalFrames[index]
    time0 = time.time()
    time1 = time.time()
    #data = {}
    windowName = "ArduCam%d"%index
    cv2.namedWindow(windowName,1)
    
    save_path = "images%d"%index
    if not os.path.exists(save_path):
        os.makedirs(save_path)
    
    print("Take picture.")
    display_time = time.time()
    rtn_val,data,rtn_cfg = ArducamSDK.Py_ArduCam_getSingleFrame(handle)
    
    if rtn_val != 0:
        print("Take picture fail,ret_val = ",rtn_val)
        return
        
    datasize = rtn_cfg['u32Size']
    if datasize == 0:
        print("data length zero!")
        return

    image = convert_image(data,rtn_cfg,color_mode)

    time1 = time.time()
    if time1 - time0 >= 1:
        print("%s %d %s\n"%("fps:",count,"/s"))
        count = 0
        time0 = time1
    count += 1
    if save_flag:
        cv2.imwrite("%s/image%d.jpg"%(save_path,totalFrame),image)
        print("Camera%d,save image%d"%(index,totalFrame))
        if save_raw:
            with open("%s/image%d.raw"%(save_path,totalFrame), 'wb') as f:
                f.write(data)
        totalFrames[index] += 1
        
    image = cv2.resize(image,(640,480),interpolation = cv2.INTER_LINEAR)

    cv2.imshow(windowName,image)
    cv2.waitKey(10)
    print("End display.")
    #print("------------------------display time:",(time.time() - display_time))

            
def showHelp():
    print(" usage: sudo python ArduCam_Ext_Trigger_Demo.py <path/config-file-name>	\
        \n\n example: sudo python ArduCam_Ext_Trigger_Demo.py .../../../python_config/AR0134_960p_Color.json	\
        \n\n")

def sigint_handler(signum, frame):
    global running
    running = False
    exit()

signal.signal(signal.SIGINT, sigint_handler)
#signal.signal(signal.SIGHUP, sigint_handler)
signal.signal(signal.SIGTERM, sigint_handler)

if __name__ == "__main__":

    config_file_name = ""
    if len(sys.argv) > 1:
        config_file_name = sys.argv[1]

        if not os.path.exists(config_file_name):
            print("Config file does not exist.")
            exit()
    else:
        showHelp()
        exit()
    
    devices_num,index,serials = ArducamSDK.Py_ArduCam_scan()
    print("Found %d devices"%devices_num)
    for i in range(devices_num):
        datas = serials[i]
        serial = "%c%c%c%c-%c%c%c%c-%c%c%c%c"%(datas[0],datas[1],datas[2],datas[3],
                                            datas[4],datas[5],datas[6],datas[7],
                                            datas[8],datas[9],datas[10],datas[11])
        print("Index:",index[i],"Serial:",serial)
        
    time.sleep(2)
    
    for i in range(devices_num):
        handle = camera_initFromFile(config_file_name,i)
        if handle != None:
            ret_val = ArducamSDK.Py_ArduCam_setMode(handle,ArducamSDK.EXTERNAL_TRIGGER_MODE)
            if(ret_val == ArducamSDK.USB_BOARD_FW_VERSION_NOT_SUPPORT_ERROR):
                print("USB_BOARD_FW_VERSION_NOT_SUPPORT_ERROR")
                exit(0)
            handles.append(handle)
            totalFrames.append(0)
        
    while running and len(handles):
        for i in range(len(handles)):
            handle = handles[i]
            value = ArducamSDK.Py_ArduCam_isFrameReady(handle)
            if value == 1:
                getAndDisplaySingleFrame(handle,i)
            #else:
            #    ArducamSDK.Py_ArduCam_softTrigger(handle)#soft trigger
        cv2.waitKey(10)

