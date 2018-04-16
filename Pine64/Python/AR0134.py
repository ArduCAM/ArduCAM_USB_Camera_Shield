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

COLOR_BYTE2RGB = 48
CAMERA_AR0134 = 0x4D091031
SensorShipAddr = 32
I2C_MODE_16_16  = 3
usbVid = 0x52cb
Width = 1280
Height = 960
cfg ={"u32CameraType":CAMERA_AR0134,
      "u32Width":Width,"u32Height":Height,
      "u32UsbVersion":1,
      "u8PixelBytes":1,
      "u16Vid":0x52cb,
      "u8PixelBits":8,
      "u32SensorShipAddr":SensorShipAddr,
      "emI2cMode":I2C_MODE_16_16 }

global saveNum,saveFlag,downFlag,flag,H_value,V_value,lx,ly,mx,my,dx,dy,W_zoom,H_zooM,handle,openFlag
openFlag = False
saveNum = 0
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

regArr=[
	#//[PLL_settings]
	[0x3028, 0x0010],		#//ROW_SPEED = 16
	[0x302A, 0x0010],		#//VT_PIX_CLK_DIV = 16
	[0x302C, 0x0001],		#//VT_SYS_CLK_DIV = 1
	[0x302E, 0x0002],		#//PRE_PLL_CLK_DIV = 2
	[0x3030, 0x0028],		#//PLL_MULTIPLIER = 40
	[0x3032, 0x0000],		#//DIGITAL_BINNING = 0
	[0x30B0, 0x0080],		#//DIGITAL_TEST = 128


	#//[Timing_settings]
	[0x301A, 0x00D8],		#//RESET_REGISTER = 216
	[0x301A, 0x10DC],		#//RESET_REGISTER = 4316
	[0x3002, 0x0000],		#//Y_ADDR_START = 0
	[0x3004, 0x0000],		#//X_ADDR_START = 0
	[0x3006, 0x03BF],		#//Y_ADDR_END = 959
	[0x3008, 0x04FF],		#//X_ADDR_END = 1279
	[0x300A, 0x0488],		#//FRAME_LENGTH_LINES = 1160
	[0x300C, 0x056C],		#//LINE_LENGTH_PCK = 1388
	[0x3012, 0x00D8],		#//COARSE_INTEGRATION_TIME = 216
	[0x3014, 0x00C0],		#//FINE_INTEGRATION_TIME = 192
	[0x30A6, 0x0001],		#//Y_ODD_INC = 1
	[0x308C, 0x0000],		#//Y_ADDR_START_CB = 0
	[0x308A, 0x0000],		#//X_ADDR_START_CB = 0
	[0x3090, 0x03BF],		#//Y_ADDR_END_CB = 959
	[0x308E, 0x04FF],		#//X_ADDR_END_CB = 1279
	[0x30AA, 0x0488],		#//FRAME_LENGTH_LINES_CB = 1160
	[0x3016, 0x00D8],		#//COARSE_INTEGRATION_TIME_CB = 216
	[0x3018, 0x00C0],		#//FINE_INTEGRATION_TIME_CB = 192
	[0x30A8, 0x0001],		#//Y_ODD_INC_CB = 1
	[0x3040, 0x4000],		#//READ_MODE = 0
	#//{0x3064, 0x1982},		#//EMBEDDED_DATA_CTRL = 6530
	[0x3064, 0x1802],
	[0x31C6, 0x8008],		#//HISPI_CONTROL_STATUS = 32776
	[0x301E, 0x0000],		#//data_pedestal
	#//{0x3100, 0x0001},		#//auto exposure

	[0x3002, 0],			#// Y_ADDR_START
	[0x3012, 150],
	
	[0x3056, 0x004A],		#// Gr_GAIN 
	[0x3058, 0x0070],		#// BLUE_GAIN
	[0x305a, 0x0070],		#// RED_GAIN 
	[0x305c, 0x004A],		#// Gb_GAIN 

	[0x3046, 0x0100],		#//en_flash/Flash indicator

	[0x301a, 0x10DC],

	[0xffff, 0xffff],
	[0xffff, 0xffff]]


ap = argparse.ArgumentParser()
ap.add_argument("-i","--type",required = False, help = "type to the image file")
args = vars(ap.parse_args())

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
	cv2.namedWindow("AR0134",1)
	cv2.setMouseCallback("AR0134",mouse_callback)
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
			print "is not available"
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
	global W_zoom,H_zoom,V_value,H_value,lx,ly,downFlag,saveFlag,saveNum
	image = Image.frombuffer("L",(Width,Height),bufferData)
	img = np.array(image)
	height,width = img.shape[:2]
	img2 = cv2.cvtColor(img,COLOR_BYTE2RGB)

	if saveFlag:
		saveFlag = False
		saveNum += 1
		name = ""
		if "bmp" == args["type"]:
			name = str(saveNum) + ".bmp"
		if "png" == args["type"]:
			name = str(saveNum) + ".png"
		if "jpg" == args["type"]:
			name = str(saveNum) + ".jpg"
		cv2.imwrite(name,img2)

	M = np.float32([[1,0,lx + H_value],[0,1,ly + V_value]])
	img3 = cv2.warpAffine(img2,M,(width,height))
	img4 = cv2.resize(img3,(width+W_zoom,height+H_zoom),interpolation = cv2.INTER_CUBIC)	
	cv2.imshow("AR0134",img4)
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
				time.sleep(0.03)
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




