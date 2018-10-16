#!/usr/bin/env python
import os
import time
import cv2
import numpy as np
import json
from ImageConvert import *
import ArducamSDK

import rospy
from cv_bridge import CvBridge, CvBridgeError
from sensor_msgs.msg import Image

from arducam_usb2_ros.srv import WriteReg, WriteRegResponse
from arducam_usb2_ros.srv import ReadReg, ReadRegResponse
from arducam_usb2_ros.srv import Trigger, TriggerResponse


global cfg,handle,Width,Heigth,color_mode
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
    ByteLength = 1
    if BitWidth > 8 and BitWidth <= 16:
        ByteLength = 2
    FmtMode = int(camera_parameter["FORMAT"][0])
    color_mode = (int)(camera_parameter["FORMAT"][1])
    print "color mode",color_mode

    I2CMode = camera_parameter["I2C_MODE"]
    I2cAddr = int(camera_parameter["I2C_ADDR"],16)
    TransLvl = int(camera_parameter["TRANS_LVL"])
    cfg = {"u32CameraType":0x4D091031,
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

def trigger(request):
    global handle,Width,Height,cfg,color_mode
    global COLOR_BayerGB2BGR,COLOR_BayerRG2BGR,COLOR_BayerGR2BGR,COLOR_BayerBG2BGR

    ArducamSDK.Py_ArduCam_softTrigger(handle)
    prev = rospy.Time.now()
    value = 0
    while value != 1:
        value = ArducamSDK.Py_ArduCam_isFrameReady(handle)
        if rospy.Time.now() - prev >= rospy.Duration(1):
            break
        rospy.sleep(0.001)
    if value == 1:
        rtn_val,data,rtn_cfg = ArducamSDK.Py_ArduCam_getSingleFrame(handle)
        if rtn_val != 0:
            return TriggerResponse("Failed to Capture")
        datasize = rtn_cfg['u32Size']
        if datasize == 0:
            return TriggerResponse("Failed to Capture")
        image = convert_image(data,rtn_cfg,color_mode)
        if h_flip:
            image = cv2.flip(image, 1)
        if v_flip:
            image = cv2.flip(image, 0)

        try:    
            img_msg = bridge.cv2_to_imgmsg(image, "bgr8")
            img_msg.header.stamp = rospy.Time.now()
            img_msg.header.frame_id = id_frame
            pub_trigger.publish(img_msg)
            return TriggerResponse("Captured")
        except CvBridgeError as e:
            return TriggerResponse("Failed to Capture")
    else:
        return TriggerResponse("Failed to Capture")

def rosShutdown():
    global handle
    rtn_val = ArducamSDK.Py_ArduCam_close(handle)
    if rtn_val == 0:
        print "device close success!"
    else:
        print "device close fail!"

if __name__ == "__main__":

    rospy.init_node("arducam_trigger_ros_node")
    pub_trigger = rospy.Publisher("arducam/triggered/camera/image_raw", Image, queue_size=1)
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
    try:
        id_frame = rospy.get_param("~frame_id")
    except:
        print "Please input frame_id."
        exit()

    if camera_initFromFile(config_file_name):
        ret_val = ArducamSDK.Py_ArduCam_setMode(handle,ArducamSDK.EXTERNAL_TRIGGER_MODE)
        if(ret_val == ArducamSDK.USB_BOARD_FW_VERSION_NOT_SUPPORT_ERROR):
            print("USB_BOARD_FW_VERSION_NOT_SUPPORT_ERROR")
            exit()

        service_write = rospy.Service('arducam/write_reg', WriteReg, write_register)
        service_read = rospy.Service('arducam/read_reg', ReadReg, read_register)
        service_capture = rospy.Service('arducam/trigger', Trigger, trigger)

        rospy.on_shutdown(rosShutdown)
        rospy.spin()
