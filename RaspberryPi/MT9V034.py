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

COLOR_BYTE2RGB = 47
CAMERA_MT9V034 = 0x4D091031
SensorShipAddr = 144
I2C_MODE_8_16  = 1
usbVid = 0x52cb
Width = 640
Height = 480
cfg ={"u32CameraType":CAMERA_MT9V034,
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

regArr=[[0x01, 0x0001], 	
	[0x02, 0x0004], 	
	[0x03, 0x01E0], 	
	[0x04, 640], 	
	[0x05, 0x005E], 	
	[0x06, 0x0039], 	
	[0x07, 0x0188], 	
	[0x08, 0x0190], 	
	[0x09, 0x01BD], 	
	[0x0A, 0x0164], 	
	[0x0B, 0x01C2], 	
	
	[0x0C, 0x0000], 	
	[0x0D, 0x0300], 	
	[0x0E, 0x0000], 	
       #[0x0F, 0x0100], 	
	[0x10, 0x0040], 	
	[0x11, 0x8042], 	
	[0x12, 0x0022], 	
	[0x13, 0x2D2E], 	
	[0x14, 0x0E02], 	
	[0x15, 0x0E32], 	
	[0x16, 0x2802], 	
	[0x17, 0x3E38], 	
	[0x18, 0x3E38], 	
	[0x19, 0x2802], 	
	[0x1A, 0x0428], 	
	[0x1B, 0x0000], 	
	[0x1C, 0x0302], 	
	[0x1D, 0x0040], 	
	[0x1E, 0x0000], 	
	[0x1F, 0x0000], 	
	[0x20, 0x03C7], 	
	[0x21, 0x0020], 	
	[0x22, 0x0020], 	
	[0x23, 0x0010], 	
	[0x24, 0x001B], 	
	[0x25, 0x001A], 	
	[0x26, 0x0004], 	
	[0x27, 0x000C], 	
	[0x28, 0x0010], 	
	[0x29, 0x0010], 	
	[0x2A, 0x0020], 	
	[0x2B, 0x0003], 	
	[0x2C, 0x0004], 	
	[0x2D, 0x0004], 	
	[0x2E, 0x0007], 	
	[0x2F, 0x0003], 	
	[0x30, 0x0003], 	
	[0x31, 0x001F], 	
	[0x32, 0x001A], 	
	[0x33, 0x0012], 	
	[0x34, 0x0003], 	
	[0x35, 0x0020], 	
	[0x36, 0x0010], 	
	[0x37, 0x0000], 	
	[0x38, 0x0000], 	
	[0x39, 0x0025], 	
	[0x3A, 0x0020], 	
	[0x3B, 0x0003], 	
	[0x3C, 0x0003], 	
	[0x46, 0x231D], 	
	[0x47, 0x0080], 	
	[0x4C, 0x0002], 	
	[0x70, 0x0000], 	
	[0x71, 0x002A], 	
	[0x72, 0x0000], 	
	[0x7F, 0x0000], 	
	[0x80, 0x04F4], 	
	[0x81, 0x04F4], 	
	[0x82, 0x04F4], 	
	[0x83, 0x04F4], 	
	[0x84, 0x04F4], 	
	[0x85, 0x04F4], 	
	[0x86, 0x04F4], 	
	[0x87, 0x04F4], 	
	[0x88, 0x04F4], 	
	[0x89, 0x04F4], 	
	[0x8A, 0x04F4], 	
	[0x8B, 0x04F4], 	
	[0x8C, 0x04F4], 	
	[0x8D, 0x04F4], 	
	[0x8E, 0x04F4], 	
	[0x8F, 0x04F4], 	
	[0x90, 0x04F4], 	
	[0x91, 0x04F4], 	
	[0x92, 0x04F4], 	
	[0x93, 0x04F4], 	
	[0x94, 0x04F4], 	
	[0x95, 0x04F4], 	
	[0x96, 0x04F4], 	
	[0x97, 0x04F4], 	
	[0x98, 0x04F4], 	
	[0x99, 0x0000], 	
	[0x9A, 0x0096], 	
	[0x9B, 0x012C], 	
	[0x9C, 0x01C2], 	
	[0x9D, 0x0258], 	
	[0x9E, 0x02F0], 	
	[0x9F, 0x0000], 	
	[0xA0, 0x0060], 	
	[0xA1, 0x00C0], 	
	[0xA2, 0x0120], 	
	[0xA3, 0x0180], 	
	[0xA4, 0x01E0], 	
	[0xA5, 0x003A], 	
	[0xA6, 0x0002], 	
	[0xA8, 0x0000], 	
	[0xA9, 0x0002], 	
	[0xAA, 0x0002], 	
	[0xAB, 0x0040], 	
	[0xAC, 0x0001], 	
	[0xAD, 0x01E0], 	
	[0xAE, 0x0014], 	
	[0xAF, 0x0003], 	
	[0xB0, 0xABE0], 	
	[0xB1, 0x0002], 	
	[0xB2, 0x0010], 	
	[0xB3, 0x0010], 	
	[0xB4, 0x0000], 	
	[0xB5, 0x0000], 	
	[0xB6, 0x0000], 	
	[0xB7, 0x0000], 	
	[0xBF, 0x0016], 	
	
	[0xC0, 0x000A], 	
	[0xC2, 0x18D0], 	
       #[0xC3, 0x007F], 	
	[0xC4, 0x007F], 	
	[0xC5, 0x007F], 	
	[0xC6, 0x0000], 	
	[0xC7, 0x4416], 	
	[0xC8, 0x4421], 	
	[0xC9, 0x0002], 	
	[0xCA, 0x0004], 	
	[0xCB, 0x01E0], 	
	[0xCC, 0x02EE], 	
	[0xCD, 0x0100], 	
	[0xCE, 0x0100], 	
	[0xCF, 0x0190], 	
	[0xD0, 0x01BD], 	
	[0xD1, 0x0064], 	
	[0xD2, 0x01C2], 	
	[0xD3, 0x0000], 	
	[0xD4, 0x0000], 	
	[0xD5, 0x0000], 	
	[0xD6, 0x0000], 	
	[0xD7, 0x0000], 	
	[0xD8, 0x0000], 	
	[0xD9, 0x0000], 	
	[0x07, 904],
	
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
	cv2.namedWindow("MT9V034",1)
	cv2.setMouseCallback("MT9V034",mouse_callback)
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

	img2 = cv2.cvtColor(img,COLOR_BYTE2RGB)
	
	if saveFlag:
		saveFlag = False
		saveNum += 1
		name = str(saveNum) + ".bmp"
		cv2.imwrite(name,img2)
	M = np.float32([[1,0,lx + H_value],[0,1,ly + V_value]])
	img3 = cv2.warpAffine(img2,M,(width,height))
	img4 = cv2.resize(img3,(width+W_zoom,height+H_zoom),interpolation = cv2.INTER_CUBIC)	

	cv2.imshow("MT9V034",img4)
	cv2.waitKey(1)

def video():
	global flag,regArr,handle
	regNum = 0
	res,handle = ArducamSDK.Py_ArduCam_autoopen(cfg)
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


