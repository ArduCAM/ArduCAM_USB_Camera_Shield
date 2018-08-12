import sys
import os
import time
import cv2
import threading
import numpy as np
import signal
import json
import Image

import ArducamSDK


global cfg,handle,running,Width,Heigth,save_flag
running = True
save_flag = False
cfg = {}
handle = {}

def configBoard(fileNodes):
    global handle
    for i in range(0,len(fileNodes)):
        fileNode = fileNodes[i]
        buffs = []
        command = fileNode[0]
        value = fileNode[1]
        index = fileNode[2]
        buffsize = fileNode[3]
        for j in range(0,len(fileNode[4])):
            buffs.append(int(fileNode[4][j],16))
        ArducamSDK.Py_ArduCam_setboardConfig(handle,int(command,16),int(value,16),int(index,16),int(buffsize,16),buffs)

pass

def JPGToMat(data,datasize):
    image = np.frombuffer(data,np.uint8,count = datasize)
    return cv2.imdecode(image,cv2.IMREAD_ANYCOLOR)

def YUVToMat(data):
    global Width,Height
    image = np.frombuffer(data, np.uint8).reshape( Height,Width , 2 )
    return cv2.cvtColor(image,cv2.COLOR_YUV2BGR_YUYV)
    
def RGB565ToMat(data):
    global Width,Height
    arr = np.frombuffer(data,dtype=np.uint16).astype(np.uint32)
    arr = ((arr & 0xFF00) >> 8) + ((arr & 0x00FF) << 8)
    arr = 0xFF000000 + ((arr & 0xF800) << 8) + ((arr & 0x07E0) << 5) + ((arr & 0x001F) << 3)

    arr.dtype = np.uint8
    iamge = arr.reshape(Height,Width,4)
    return cv2.flip(iamge,0)

def writeSensorRegs(fileNodes):
    global handle
    for i in range(0,len(fileNodes)):
        fileNode = fileNodes[i]
        regAddr = int(fileNode[0],16)
        val = int(fileNode[1],16)
        ArducamSDK.Py_ArduCam_writeSensorReg(handle,regAddr,val)

pass

def camera_initFromFile(fialeName):
    global cfg,handle,Width,Height
    #load config file
    config = json.load(open(fialeName,"r"))

    camera_parameter = config["camera_parameter"]
    Width = int(camera_parameter["SIZE"][0])
    Height = int(camera_parameter["SIZE"][1])

    BitWidth = camera_parameter["BIT_WIDTH"]
    FmtMode = camera_parameter["FORMAT"]
    I2CMode = camera_parameter["I2C_MODE"]
    SensorShipAddr = int(camera_parameter["I2C_ADDR"],16)

    cfg = {"u32CameraType":0x4D091031,
            "u32Width":Width,"u32Height":Height,
            "u32UsbVersion":0,
            "u8PixelBytes":1,
            "u16Vid":0,
            "u32Size":0,
            "u8PixelBits":BitWidth,
            "u32SensorShipAddr":SensorShipAddr,
            "emI2cMode":I2CMode,
            "emImageFmtMode":FmtMode }

    ret,handle,rtn_cfg = ArducamSDK.Py_ArduCam_autoopen(cfg)
    if ret == 0:
       
        #ArducamSDK.Py_ArduCam_writeReg_8_8(handle,0x46,3,0x00)
        usb_version = rtn_cfg['u32UsbVersion']
        print "USB VERSION:",usb_version
        #config board param
        configBoard(config["board_parameter"])

        if usb_version == ArducamSDK.USB_1 or usb_version == ArducamSDK.USB_2:
            configBoard(config["board_parameter_dev2"])
        if usb_version == ArducamSDK.USB_3:
            configBoard(config["board_parameter_dev3_inf3"])
        if usb_version == ArducamSDK.USB_3_2:
            configBoard(config["board_parameter_dev3_inf2"])
        
        writeSensorRegs(config["register_parameter"])
        
        if usb_version == ArducamSDK.USB_3:
            writeSensorRegs(config["register_parameter_dev3_inf3"])
        if usb_version == ArducamSDK.USB_3_2:
            writeSensorRegs(config["register_parameter_dev3_inf2"])

        rtn_val,datas = ArducamSDK.Py_ArduCam_readUserData(handle,0x400-16, 16)
        print "Serial: %c%c%c%c-%c%c%c%c-%c%c%c%c"%(datas[0],datas[1],datas[2],datas[3],
                                                    datas[4],datas[5],datas[6],datas[7],
                                                    datas[8],datas[9],datas[10],datas[11])

        return True
    else:
        print "open fail"
        return False

pass
       
def captureImage_thread():
    global handle,running

    rtn_val = ArducamSDK.Py_ArduCam_beginCaptureImage(handle)
    if rtn_val != 0:
        print "Error beginning capture, rtn_val = ",rtn_val
        running = False
        return
    else:
        print "Capture began, rtn_val = ",rtn_val
    
    #time.sleep(0.1)
    ArducamSDK.Py_ArduCam_writeReg_8_8(handle,0x46,3,0xC0)
    ArducamSDK.Py_ArduCam_writeReg_8_8(handle,0x46,3,0x40)
    ArducamSDK.Py_ArduCam_writeReg_8_8(handle,0x46,3,0x00)
    while running:
        #print "capture"
        rtn_val = ArducamSDK.Py_ArduCam_captureImage(handle)
        if rtn_val != 0:
            print "Error capture image, rtn_val = ",rtn_val
            break
        time.sleep(0.005)
        
    running = False
    ArducamSDK.Py_ArduCam_endCaptureImage(handle)

def readImage_thread():
    global handle,running,Width,Height,save_flag,cfg
    count = 0
    totalFrame = 0
    time0 = time.time()
    time1 = time.time()
    data = {}
    cv2.namedWindow("ArduCam Demo",1)
    if not os.path.exists("images"):
        os.makedirs("images")
    while running:
        display_time = time.time()
        if ArducamSDK.Py_ArduCam_availableImage(handle) > 0:		
            rtn_val,data,rtn_cfg = ArducamSDK.Py_ArduCam_readImage(handle)
            datasize = rtn_cfg['u32Size']
            if rtn_val != 0:
                print "read data fail!"
                continue
                
            if datasize == 0:
                continue

            image = None
            emImageFmtMode = cfg['emImageFmtMode']
            if emImageFmtMode == ArducamSDK.FORMAT_MODE_JPG:
                image = JPGToMat(data,datasize)
            if emImageFmtMode == ArducamSDK.FORMAT_MODE_YUV:
                image = YUVToMat(data)
            if emImageFmtMode == ArducamSDK.FORMAT_MODE_RGB:
                image = RGB565ToMat(data)
            if emImageFmtMode == ArducamSDK.FORMAT_MODE_MON:
                image = np.frombuffer(data, np.uint8).reshape( Height,Width , 1 )
            if emImageFmtMode == ArducamSDK.FORMAT_MODE_RAW:
                image = np.frombuffer(data, np.uint8).reshape( Height,Width , 1 )
                image = cv2.cvtColor(image,cv2.COLOR_BayerGR2RGB)
        
            time1 = time.time()
            if time1 - time0 >= 1:
                print "%s %d %s\n"%("fps:",count,"/s")
                count = 0
                time0 = time1
            count += 1
            if save_flag:
                cv2.imwrite("images/image%d.jpg"%totalFrame,image)
                totalFrame += 1
                
            image = cv2.resize(image,(640,480),interpolation = cv2.INTER_LINEAR)

            cv2.imshow("ArduCam Demo",image)
            cv2.waitKey(10)
            ArducamSDK.Py_ArduCam_del(handle)
        #print "------------------------display time:",(time.time() - display_time)

def showHelp():
    print " usage: sudo python ArduCam_Py_Demo.py <path/config-file-name>	\
        \n\n example: sudo python ArduCam_Py_Demo.py ../Config/AR0134_960p_Color.yml	\
        \n\n While the program is running, you can press the following buttons in the terminal:	\
        \n\n 's' + Enter:Save the image to the images folder.	\
        \n\n 'c' + Enter:Stop saving images.	\
        \n\n 'q' + Enter:Stop running the program.	\
        \n\n"

def sigint_handler(signum, frame):
    global running,handle
    running = False
    exit()

signal.signal(signal.SIGINT, sigint_handler)
signal.signal(signal.SIGHUP, sigint_handler)
signal.signal(signal.SIGTERM, sigint_handler)

if __name__ == "__main__":

    config_file_name = ""
    if len(sys.argv) > 1:
        config_file_name = sys.argv[1]

        if not os.path.exists(config_file_name):
            print "Config file does not exist."
            exit()
    else:
        showHelp()
        exit()

    if camera_initFromFile(config_file_name):
        ct = threading.Thread(target=captureImage_thread)
        rt = threading.Thread(target=readImage_thread)
        ct.start()
        rt.start()
#
        while running:
            input_kb = str(sys.stdin.readline()).strip("\n")

            if input_kb == 'q' or input_kb == 'Q':
                running = False
            if input_kb == 's' or input_kb == 'S':
                save_flag = True
            if input_kb == 'c' or input_kb == 'C':
                save_flag = False

        ct.join()
        rt.join()
        #pause
        #ArducamSDK.Py_ArduCam_writeReg_8_8(handle,0x46,3,0x40)
        rtn_val = ArducamSDK.Py_ArduCam_close(handle)
        if rtn_val == 0:
            print "device close success!"
        else:
            print "device close fail!"

        #os.system("pause")
