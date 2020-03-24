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
from arducam_usb2_ros.srv import Capture, CaptureResponse


global cfg,handle,Width,Heigth,color_mode
cfg = {}
handle = {}


def configBoard(config):
    global handle
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
    cfg = {"u32CameraType": 0x00,
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
                configBoard(configs[i])

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
    global handle,Width,Height,cfg,color_mode
    global COLOR_BayerGB2BGR,COLOR_BayerRG2BGR,COLOR_BayerGR2BGR,COLOR_BayerBG2BGR

    rtn_val = ArducamSDK.Py_ArduCam_captureImage(handle)
    if rtn_val > 255:
        print "Error capture image, rtn_val = ",rtn_val
        if rtn_val == ArducamSDK.USB_CAMERA_USB_TASK_ERROR:
            exit()
    if ArducamSDK.Py_ArduCam_availableImage(handle) > 0:		
        rtn_val,data,rtn_cfg = ArducamSDK.Py_ArduCam_readImage(handle)
        datasize = rtn_cfg['u32Size']
        if rtn_val != 0 or datasize == 0:
            print "read data fail!"
            ArducamSDK.Py_ArduCam_del(handle)
            return

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
            pub.publish(img_msg)
            cv2.waitKey(10)
        except CvBridgeError as e:
            print e
            pass
        ArducamSDK.Py_ArduCam_del(handle)
    else:
        time.sleep(0.001)

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
    global handle,Width,Height,cfg,color_mode
    global COLOR_BayerGB2BGR,COLOR_BayerRG2BGR,COLOR_BayerGR2BGR,COLOR_BayerBG2BGR
    
    if ArducamSDK.Py_ArduCam_availableImage(handle) > 0:		
        rtn_val,data,rtn_cfg = ArducamSDK.Py_ArduCam_readImage(handle)
        datasize = rtn_cfg['u32Size']
        if rtn_val != 0:
            print "read data fail!"
            return CaptureResponse("Failed to Capture")
            
        if datasize == 0:
            return CaptureResponse("Failed to Capture")

        image = convert_image(data,rtn_cfg,color_mode)
        if h_flip:
            image = cv2.flip(image, 1)
        if v_flip:
            image = cv2.flip(image, 0)

        try:    
            img_msg = bridge.cv2_to_imgmsg(image, "bgr8")
            img_msg.header.stamp = rospy.Time.now()
            img_msg.header.frame_id = id_frame
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
    pub_capture = rospy.Publisher("arducam/captured/camera/image_raw", Image, queue_size=1)
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
