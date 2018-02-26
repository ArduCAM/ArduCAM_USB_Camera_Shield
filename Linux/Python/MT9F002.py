from ctypes import *

import ctypes

import os
import cv2
import time
import Image
import argparse
import numpy as np
import thread as thread

from select import select
from evdev import InputDevice

import ArducamSDK

COLOR_BYTE2RGB = 49
CAMERA_MT9F002 = 0x4D091031
SensorShipAddr = 32
I2C_MODE_16_16  = 3
usbVid = 0x52cb
Width = 4384
Height = 3288
cfg ={"u32CameraType":CAMERA_MT9F002,
      "u32Width":Width,"u32Height":Height,
      "u32UsbVersion":1,
      "u8PixelBytes":1,
      "u16Vid":0x52cb,
      "u8PixelBits":8,
      "u32SensorShipAddr":SensorShipAddr,
      "emI2cMode":I2C_MODE_16_16 }

global saveNum,saveFlag,downFlag,flag,handle,openFlag
openFlag = False
saveNum = 0
handle = {}
downFlag = False
flag = True
saveFlag = False


regArr= [[0x301A, 0x0018],
	 
	[0x0103, 0x1   ],  
	     
	[0x31AE, 0x301 ],  
	[0x0112, 0x0C0C],  	
	[0x301A, 0x0010],  	
	[0x3064, 0x0805],  	
	[0x301E, 0x00A8],  	
	[0x301A, 0x10DC],  	
	[0x0104, 0x1   ],  
	[0x0100, 0x0   ],  

	[0x0300, 0x4   ],  
	[0x0302, 0x01  ],  
	[0x0304, 0x03  ],  
	[0x0306, 0x6E  ],  
	[0x0308, 0x0C  ],  
	[0x030A, 0x01  ],  

	[0x3016, 0x111 ],  
	[0x0344, 0x090 ],  
	[0x0348, 0x11AF],  	
	[0x0346, 0x020 ],  
	[0x034A, 0xCF7 ],  
	[0x3040, 0x0041],  	
	[0x0400, 0x0   ],  
	[0x0404, 0x10  ],  
	[0x034C, 0x1120],  	
	[0x034E, 0xCD8 ],  
	[0x0342, 0x33C1],  	
	[0x0340, 0x0D67],  	
	[0x3010, 0x00CF],  	
	[0x3012, 0x0010],  	
	[0x3014, 0x1F50],  	
	[0x3018, 0x0000],  	
	
	[0x0104, 0x0],	
	[0x0100, 0x1],	
	[0x0304, 8],
	
	[0x301a, 0x5ccc],        
	[0x3012, 500],
	[0x0206, 33],
	[0x0208, 50],
	[0x020a, 50],
	[0x020c, 33],

	[0xffff, 0xffff],
	[0xffff, 0xffff]
]


ap = argparse.ArgumentParser()
ap.add_argument("--type",default = "jpg",required = False, help = "type to the image file")
ap.add_argument("--name",required = False, help = "name to the image file")
args = vars(ap.parse_args())

def readThread(threadName,read_Flag):
	global flag,handle
	data = {}
	cv2.namedWindow("MT9F002",1)
	while flag:
		if ArducamSDK.Py_ArduCam_available(handle) > 0:
			
			res,data = ArducamSDK.Py_ArduCam_read(handle,Width * Height)
			if res == 0:
				ArducamSDK.Py_ArduCam_del(handle)
			else:
				print "read data fail!"
			
		else:
			print "is not availiable"
		if len(data) >= Width * Height:
			show(data)
			flag = False
		else:
			print "data length is not enough!"
		if flag == False:		
			break
	
thread.start_new_thread( readThread,("Thread-2", flag,))

pass

def show(bufferData):
	global downFlag,saveFlag,saveNum
	image = Image.frombuffer("L",(Width,Height),bufferData)
	img = np.array(image)
	height,width = img.shape[:2]
	img2 = cv2.cvtColor(img,COLOR_BYTE2RGB)
        saveNum += 1
	
	if args["name"] != "":
                name = args["name"]   
        else:
                name = str(saveNum)
	if "bmp" == args["type"]:        
                name += ".bmp"
        if "png" == args["type"]:        
                name += ".png"
        if "jpg" == args["type"]:        
                name += ".jpg"
	cv2.imwrite(name,img2)	


def video():
	global flag,regArr,handle
	regNum = 0
	res,handle = ArducamSDK.Py_ArduCam_autoopen(cfg)
	capFlag = True
	if res == 0:
		openFlag = True
		print "device open success!"
		while (regArr[regNum][0] != 0xFFFF):
			ArducamSDK.Py_ArduCam_writeSensorReg(handle,regArr[regNum][0],regArr[regNum][1])
			regNum = regNum + 1
		res = ArducamSDK.Py_ArduCam_beginCapture(handle)
		
		if res == 0:
			print "transfer task create success!"
			while flag :
                                if capFlag:
                                        res = ArducamSDK.Py_ArduCam_capture(handle)
                                        if res != 0:
                                                print "capture fail!"
                                                break
                                        else:
                                                capFlag = False
                                time.sleep(0.4)
				if flag == False:		
					break
		else:
			print "transfer task create fail!"
		res = ArducamSDK.Py_ArduCam_close(handle)
		if res == 0:
			openFlag = False
			print "device close success!"
		else:
			print "device close fail!"
	else:
		print "device open fail!"

if __name__ == "__main__":		
	video()



