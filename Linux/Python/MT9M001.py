from ctypes import *

import ctypes

import os
import cv2
import time
import Image
import numpy as np
import thread as thread

from select import select
from evdev import InputDevice

import ArducamSDK


CAMERA_MT9M001 = 0x4D091031
SensorShipAddr = 0xBA
I2C_MODE_8_16  = 1
usbVid = 0x52cb
Width = 1280
Height = 1024
cfg ={"u32CameraType":CAMERA_MT9M001,
      "u32Width":Width,"u32Height":Height,
      "u32UsbVersion":1,
      "u8PixelBytes":1,
      "u16Vid":0x52cb,
      "u8PixelBits":8,
      "u32SensorShipAddr":SensorShipAddr,
      "emI2cMode":I2C_MODE_8_16 }


global saveFlag,downFlag,flag,H_value,V_value,lx,ly,mx,my,dx,dy,W_zoom,H_zooM,handle,openFlag
openFlag = False
handle = {}
downFlag = False
flag = True
saveFlag = False
H_value = 0
V_value = 0
W_zoom = 0
H_zoom = 0
lx = 0
ly = 0
mx = 0
my = 0
dx = 0
dy = 0

regArr=[[0x0009, 0x0419], #shutter speed
	[0x0035, 0x0018], #global gain
	[0xFFFF, 0xFFFF]
]


def mouse_callback(event,x,y,flags,param):
	global downFlag,mx,my,dx,dy,H_value,V_value,lx,ly

	if event == cv2.EVENT_LBUTTONDOWN:
		downFlag = True
		dx = x
		dy = y
	if event == cv2.EVENT_LBUTTONUP:
		downFlag = False
		lx += H_value
		ly += V_value
		H_value = 0 
		V_value = 0
		
	if event == cv2.EVENT_MOUSEMOVE:
		mx = x
		my = y
		if downFlag:
			H_value = mx - dx
			V_value = my - dy

def detectInputKey(threadName,view_Flag):
	global flag,W_zoom,H_zoom,saveFlag,H_value,V_value,lx,ly,data
	dev = InputDevice("/dev/input/event0")
	while True:
		select([dev],[],[])
		for event in dev.read():

			if(event.value == 1) and event.code != 0:

				if event.code == 16:
					flag = False	
					return
				if event.code == 19:
					H_value = 0 
					V_value = 0
					lx = 0
					ly = 0

				if event.code == 31:
					saveFlag = True
				if event.code == 105:	
						if (Width + W_zoom) > (Width/10*1.5 ):
							W_zoom -= Width/10
						if (Height + H_zoom) > (Height/10*1.5):
							H_zoom -= Height/10
			
				if event.code == 106:			
						W_zoom += Width/10		
						H_zoom += Height/10
	
thread.start_new_thread( detectInputKey,("Thread-1", flag,))

pass

def readThread(threadName,read_Flag):
	global flag,handle
	count = 0
	time0 = time.time()
	time1 = time.time()
	data = {}
	cv2.namedWindow("MT9M001",1)
	cv2.setMouseCallback("MT9M001",mouse_callback)
	while flag:
		if ArducamSDK.Py_ArduCam_available(handle) > 0:
			
			res,data = ArducamSDK.Py_ArduCam_read(handle,Width * Height)
			if res == 0:
				count += 1
				time1 = time.time()
				ArducamSDK.Py_ArduCam_del(handle)
			else:
				print "read data fail!"
			
		else:
			print "is not availiable"
		if len(data) >= Width * Height:
			if time1 - time0 >= 1:
				print "%s %d %s\n"%("fps:",count,"/s")
				count = 0
				time0 = time1
			show(data)
		else:
			print "data length is not enough!"
		if flag == False:		
			break
	
thread.start_new_thread( readThread,("Thread-2", flag,))

pass

def show(bufferData):
	global W_zoom,H_zoom,V_value,H_value,lx,ly,downFlag,saveFlag
	image = Image.frombuffer("L",(Width,Height),bufferData)
	img = np.array(image)
	height,width = img.shape[:2]
	#COLOR_BAYER_BG2RGB
	#COLOR_BAYER_GB2RGB
	#COLOR_BAYER_GR2RGB
	#COLOR_BAYER_RG2RGB
	img2 = cv2.cvtColor(img,cv2.COLOR_BAYER_BG2RGB)
	
	if saveFlag:
		saveFlag = False
		saveNum += 1
		name = str(saveNum) + ".bmp"
		cv2.imwrite(name,img2)
	M = np.float32([[1,0,lx + H_value],[0,1,ly + V_value]])
	img3 = cv2.warpAffine(img2,M,(width,height))
	img4 = cv2.resize(img3,(width+W_zoom,height+H_zoom),interpolation = cv2.INTER_CUBIC)	

	cv2.imshow("MT9M001",img4)
	cv2.waitKey(1)

def video():
	global flag,regArr,handle
	regNum = 0
	res,handle = ArducamSDK.Py_ArduCam_autoopen(cfg)
	if res == 0:
		openFlag = True
		print "device open success!"
		ArducamSDK.Py_ArduCam_writeReg_8_8(handle,0x46,0x01,0x25)
		while (regArr[regNum][0] != 0xFFFF):
			ArducamSDK.Py_ArduCam_writeSensorReg(handle,regArr[regNum][0],regArr[regNum][1])
			regNum = regNum + 1
		res = ArducamSDK.Py_ArduCam_beginCapture(handle)
		
		if res == 0:
			print "transfer task create success!"
			while flag :		
				res = ArducamSDK.Py_ArduCam_capture(handle)
				if res != 0:
					print "capture fail!"
					break
				time.sleep(0.005)
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


