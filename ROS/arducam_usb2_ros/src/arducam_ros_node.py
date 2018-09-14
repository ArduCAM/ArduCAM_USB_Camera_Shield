#!/usr/bin/env python
import sys
import os
import time
import cv2
import threading
import numpy as np
import signal
import json

import ArducamSDK

import rospy
from cv_bridge import CvBridge, CvBridgeError
from sensor_msgs.msg import Image

from arducam_usb2_ros.srv import WriteReg, WriteRegResponse
from arducam_usb2_ros.srv import ReadReg, ReadRegResponse
from arducam_usb2_ros.srv import Capture, CaptureResponse


global cfg,handle,ruWidth,Heigth,color_mode
cfg = {}
handle = {}
global COLOR_BayerGB2BGR,COLOR_BayerRG2BGR,COLOR_BayerGR2BGR,COLOR_BayerBG2BGR

COLOR_BayerBG2BGR = 46
COLOR_BayerGB2BGR = 47
COLOR_BayerRG2BGR = 48
COLOR_BayerGR2BGR = 49


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
        if fileNode[0] == "DELAY":
            time.sleep(float(fileNode[1])/1000)
            continue
        regAddr = int(fileNode[0],16)
        val = int(fileNode[1],16)
        ArducamSDK.Py_ArduCam_writeSensorReg(handle,regAddr,val)

pass

def camera_initFromFile(fialeName):
    global cfg,handle,Width,Height,color_mode
    #load config file
    config = json.load(open(fialeName,"r"))

    camera_parameter = config["camera_parameter"]
    Width = int(camera_parameter["SIZE"][0])
    Height = int(camera_parameter["SIZE"][1])

    BitWidth = camera_parameter["BIT_WIDTH"]
    FmtMode = int(camera_parameter["FORMAT"][0])
    color_mode = (int)(camera_parameter["FORMAT"][1])
    print "color mode",color_mode

    I2CMode = camera_parameter["I2C_MODE"]
    I2cAddr = int(camera_parameter["I2C_ADDR"],16)

    cfg = {"u32CameraType":0x4D091031,
            "u32Width":Width,"u32Height":Height,
            "usbType":0,
            "u8PixelBytes":1,
            "u16Vid":0,
            "u32Size":0,
            "u8PixelBits":BitWidth,
            "u32I2cAddr":I2cAddr,
            "emI2cMode":I2CMode,
            "emImageFmtMode":FmtMode,
            "u32TransLvl":32 }

    # ArducamSDK.

    if serial_selection == None:
        ret,handle,rtn_cfg = ArducamSDK.Py_ArduCam_autoopen(cfg)
    else:
        index = None
        num_cam, cam_list, cam_serial = ArducamSDK.Py_ArduCam_scan()
        for i in range(num_cam):
            datas = cam_serial[i]
            camera_serial = "%c%c%c%c-%c%c%c%c-%c%c%c%c"%(datas[0],datas[1],datas[2],datas[3],
                                                          datas[4],datas[5],datas[6],datas[7],
                                                          datas[8],datas[9],datas[10],datas[11])
            if camera_serial == str(serial_selection):
                print("Arducam " + str(serial_selection) + " found")
                index = i
                break
        if index == None:
            print("Arducam " + str(serial_selection) + " not found")
            exit()
        time.sleep(3) 
        ret,handle,rtn_cfg = ArducamSDK.Py_ArduCam_open(cfg,i)
                
    if ret == 0:
        usb_version = rtn_cfg['usbType']
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
        print "open fail,rtn_val = ",ret
        return False

pass
       
def captureImage():
    global handle

    rtn_val = ArducamSDK.Py_ArduCam_captureImage(handle)
    if rtn_val > 255:
        print "Error capture image, rtn_val = ",rtn_val
        if rtn_val != ArducamSDK.USB_CAMERA_FRAME_INDEX_ERROR and rtn_val != ArducamSDK.USB_CAMERA_DATA_LEN_ERROR:
            return
    if ArducamSDK.Py_ArduCam_availableImage(handle) > 0:		
        rtn_val,data,rtn_cfg = ArducamSDK.Py_ArduCam_readImage(handle)
        datasize = rtn_cfg['u32Size']
        if rtn_val != 0:
            print "read data fail!"
            return
            
        if datasize == 0:
            return

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
        if h_flip:
            image = cv2.flip(image, 1)
        if v_flip:
            image = cv2.flip(image, 0)

        try:    
            img_msg = bridge.cv2_to_imgmsg(image, "bgr8")
            img_msg.header.stamp = rospy.Time.now()
            img_msg.header.frame_id = "arducam_optical_frame"
            pub.publish(img_msg)
            cv2.waitKey(10)
        except CvBridgeError as e:
            pass
        ArducamSDK.Py_ArduCam_del(handle)
    else:
        time.sleep(0.010)

def write_register(request):
    global handle
    register = request.register
    value = request.value
    rtn_val = ArducamSDK.Py_ArduCam_writeSensorReg(handle,register,value)
    if rtn_val == 0:
        output = 'Value %d written to register %d' % (value, register)
    else:
        output = 'Invalid register'
    return WriteRegResponse(output)

def read_register(request):
    global handle
    register = request.register
    rtn_val, output = ArducamSDK.Py_ArduCam_readSensorReg(handle,register)
    if rtn_val == 0:
        output = 'Register %d: %d' % (register, output)
    else:
        output = 'Invalid register'
    return ReadRegResponse(output)

def capture(request):
    global handle
    if ArducamSDK.Py_ArduCam_availableImage(handle) > 0:		
        rtn_val,data,rtn_cfg = ArducamSDK.Py_ArduCam_readImage(handle)
        datasize = rtn_cfg['u32Size']
        if rtn_val != 0:
            print "read data fail!"
            return CaptureResponse("Failed to Capture")
            
        if datasize == 0:
            return CaptureResponse("Failed to Capture")

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
        if h_flip:
            image = cv2.flip(image, 1)
        if v_flip:
            image = cv2.flip(image, 0)

        try:    
            img_msg = bridge.cv2_to_imgmsg(image, "bgr8")
            img_msg.header.stamp = rospy.Time.now()
            img_msg.header.frame_id = "arducam_optical_frame"
            pub_capture.publish(img_msg)
            return CaptureResponse("Captured")
        except CvBridgeError as e:
            return CaptureResponse("Failed to Capture")
    else:
        return CaptureResponse("Failed to Capture")

def rosShutdown():
    global handle
    ArducamSDK.Py_ArduCam_endCaptureImage(handle)
    rtn_val = ArducamSDK.Py_ArduCam_close(handle)
    if rtn_val == 0:
        print "device close success!"
    else:
        print "device close fail!"

if __name__ == "__main__":

    rospy.init_node("arducam_ros_node")
    pub = rospy.Publisher("arducam/camera/image_raw", Image, queue_size=1)
    pub_capture = rospy.Publisher("arducam/camera/captured", Image, queue_size=1)
    bridge = CvBridge()
    try:
        config_file_name = rospy.get_param("~config_file")
        if not os.path.exists(config_file_name):
            print "Config file does not exist."
            exit()
    except:
        print("Empty config_file parameter.")
        exit()
    try:
        serial_selection = rospy.get_param("~camera_serial")
        if len(serial_selection) == 0:
            raise
    except:
        serial_selection = None
    try:
        h_flip = rospy.get_param("~horizontal_flip")
    except:
        h_flip = False
    try:
        v_flip = rospy.get_param("~vertical_flip")
    except:
        v_flip = False

    if camera_initFromFile(config_file_name):
        ArducamSDK.Py_ArduCam_setMode(handle,ArducamSDK.CONTINUOUS_MODE)
        rtn_val = ArducamSDK.Py_ArduCam_beginCaptureImage(handle)
        if rtn_val != 0:
            print "Error beginning capture, rtn_val = ",rtn_val
            exit()
        else:
            print "Capture began, rtn_val = ",rtn_val

        service_write = rospy.Service('arducam/write_reg', WriteReg, write_register)
        service_read = rospy.Service('arducam/read_reg', ReadReg, read_register)
        service_capture = rospy.Service('arducam/capture', Capture, capture)

        rospy.on_shutdown(rosShutdown)
        while not rospy.is_shutdown():
            captureImage()
