#!/usr/bin/env python
import os
import time
import cv2
import numpy as np
import json
from ImageConvert import *
import arducam_config_parser
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


def configBoard(handle, config):
    ArducamSDK.Py_ArduCam_setboardConfig(handle, config.params[0], \
        config.params[1], config.params[2], config.params[3], \
            config.params[4:config.params_length])

pass

def camera_initFromFile(fileName):
    global cfg,handle,Width,Height,color_mode
    #load config file
    config = arducam_config_parser.LoadConfigFile(fileName)
    
    camera_parameter = config.camera_param.getdict()
    Width = camera_parameter["WIDTH"]
    Height = camera_parameter["HEIGHT"]

    BitWidth = camera_parameter["BIT_WIDTH"]
    ByteLength = 1
    if BitWidth > 8 and BitWidth <= 16:
        ByteLength = 2
    FmtMode = camera_parameter["FORMAT"][0]
    color_mode = camera_parameter["FORMAT"][1]
    print "color mode",color_mode

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
    global handle,soft_trigger,captured
    soft_trigger = True
    ArducamSDK.Py_ArduCam_softTrigger(handle)
    while True:
        if captured == 0:
            continue
        elif captured == 1:
            captured = 0
            return TriggerResponse("Failed to Capture")
        elif captured == 2:
            captured = 0
            return TriggerResponse("Captured")

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
        service_trigger = rospy.Service('arducam/trigger', Trigger, trigger)

        captured = 0
        soft_trigger = False
        rospy.on_shutdown(rosShutdown)
        while not rospy.is_shutdown():
            value = ArducamSDK.Py_ArduCam_isFrameReady(handle)
            if value == 1:
                rtn_val,data,rtn_cfg = ArducamSDK.Py_ArduCam_getSingleFrame(handle)
                if rtn_val != 0:
                    rospy.sleep(0.001)
                    if soft_trigger:
                        captured = 1
                        soft_trigger = False
                    continue
                datasize = rtn_cfg['u32Size']
                if datasize == 0:
                    rospy.sleep(0.001)
                    if soft_trigger:
                        captured = 1
                        soft_trigger = False
                    continue
                image = convert_image(data,rtn_cfg,color_mode)
                if h_flip:
                    image = cv2.flip(image, 1)
                if v_flip:
                    image = cv2.flip(image, 0)

                try:    
                    # img_msg = bridge.cv2_to_imgmsg(image, "bgr8")
                    img_msg = bridge.cv2_to_imgmsg(image)
                    img_msg.header.stamp = rospy.Time.now()
                    img_msg.header.frame_id = id_frame
                    pub_trigger.publish(img_msg)
                    if soft_trigger:
                        captured = 2
                        soft_trigger = False
                except CvBridgeError as e:
                    rospy.sleep(0.001)
                    if soft_trigger:
                        captured = 1
                        soft_trigger = False
            else:
                rospy.sleep(0.001)
