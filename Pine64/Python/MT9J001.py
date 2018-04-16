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
CAMERA_MT9J001 = 0x4D091031
SensorShipAddr = 32
I2C_MODE_16_16  = 3
usbVid = 0x52cb
Width = 3664
Height = 2748
cfg ={"u32CameraType":CAMERA_MT9J001,
      "u32Width":Width,"u32Height":Height,
      "u32UsbVersion":1,
      "u8PixelBytes":1,
      "u16Vid":0x52cb,
      "u8PixelBits":8,
      "u32SensorShipAddr":SensorShipAddr,
      "emI2cMode":I2C_MODE_16_16 }


global saveFlag,downFlag,flag,H_value,V_value,lx,ly,mx,my,dx,dy,W_zoom,H_zooM,handle,openFlag
openFlag = False
handle = {}
downFlag = False
flag = True
saveFlag = False
H_value = 0
V_value = 0
W_zoom = Width/10*-8
H_zoom = Height/10*-8
lx = 0
ly = 0
mx = 0
my = 0
dx = 0
dy = 0

regArr=[[0x316C, 0x0429], 
	[0x3174, 0x8000],
	[0x3E40, 0xDC05], 
	[0x3E42, 0x6E22], 
	[0x3E44, 0xDC22], 
	[0x3E46, 0xFF00], 
	[0x3ED4, 0xF998], 
	[0x3ED6, 0x9789], 
	[0x3EDE, 0xE41A], 
	[0x3EE0, 0xA43F], 
	[0x3EE2, 0xA4BF], 
	[0x3EEC, 0x1221], 
	[0x3EEE, 0x1224], 

	[0x0100, 0x0	], 
	[0x0300, 0x5	], 
	[0x0302, 0x08	], 
	[0x0304, 0x05	], 
	[0x0306, 0x84	], 
	[0x0308, 0x0A	], 
	[0x030A, 0x04	], 
	
	[0x3064, 0x805	], 
	[0x0104, 0x1	], 
	[0x3016, 0x111	], 
	[0x0344, 0x518	], 
	[0x0348, 0xA15	], 
	[0x0346, 0x386	], 
	[0x034A, 0x743	], 
	[0x3040, 0x04C3	], 
	[0x0400, 0x0	], 
	[0x0404, 0x10	], 
	[0x034C, 0x280	], 
	[0x034E, 0x1E0	],
	[0x0342, 0x0D12	], 
	[0x0340, 0x0230	], 
	[0x0202, 0x0010	], 
	[0x3014, 0x07B2	], 
	[0x3010, 0x0134	], 
	[0x3018, 0x0000	], 
	[0x30D4, 0x1080	], 
	[0x306E, 0x90B0	], 
	[0x3E00, 0x0010	], 
	[0x3178, 0x0000	], 
	[0x3ED0, 0x1B24	], 
	[0x3EDC, 0xC3E4	], 
	[0x3EE8, 0x0000	], 
	[0x0104, 0x0	], 
	[0x0100, 0x1	], 

	[0x0100, 0x0	],
	[0x0300, 0x6	],  
	[0x0302, 0x01	],  
	[0x0304, 0x08	],  
	[0x0306, 0x6E	],  
	[0x0308, 0x0C	],  
	[0x030A, 0x01	],   
	         
	[0x3064, 0x805	],   
	[0x0104, 0x1	],   
	[0x3016, 0x111	],   
	[0x0344, 0x070	],   
	[0x0348, 0xEBF	],   
	[0x0346, 0x008	],   
	[0x034A, 0xAC3	],   
	[0x3040, 0x0041	],   
	[0x0400, 0x0	],   
	[0x0404, 0x10	],  
	[0x034C, 0xE50	],   
	[0x034E, 0xABC	],   
	[0x0342, 0x1D10	],  
	[0x0340, 0x0B4D	],   
	[0x0202, 0x0010	],   
	[0x3014, 0x03F2	],  
	[0x3010, 0x009C	],   
	[0x3018, 0x0000	],   
	[0x30D4, 0x1080	],   
	[0x306E, 0x90B0	],   
	[0x3E00, 0x0010	],  
	[0x3178, 0x0000	],  
	[0x3ED0, 0x1B24	],   
	[0x3EDC, 0xC3E4	],  
	[0x3EE8, 0x0000	],  
	[0x0104, 0x0	],  
	[0x0100, 0x1	],   
	
	[0x31AE, 0x0301 ], 

	[0x301A, 0x0010], 
	[0x3064, 0x0805], 
	[0x301E, 0x00A8], 

	[0x301A, 0x10DC],
	[0x0206, 0x001a ],
	[0x0208, 0x002a ],
	[0x020A, 0x002a ],
	[0x020C, 0x001a ],
	[0x3012, 0x0200 ],
	[0xffff, 0xffff]]


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
	cv2.namedWindow("MT9J001",1)
	cv2.setMouseCallback("MT9J001",mouse_callback)
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
	global W_zoom,H_zoom,V_value,H_value,lx,ly,downFlag,saveFlag
	image = Image.frombuffer("L",(Width,Height),bufferData)
	img = np.array(image)
	height,width = img.shape[:2]
	img2 = cv2.cvtColor(img,47)

	if saveFlag:
		saveFlag = False
		saveNum += 1
		name = str(saveNum) + ".bmp"
		cv2.imwrite(name,img2)
	M = np.float32([[1,0,lx + H_value],[0,1,ly + V_value]])
	img3 = cv2.warpAffine(img2,M,(width,height))
	img4 = cv2.resize(img3,(width+W_zoom,height+H_zoom),interpolation = cv2.INTER_CUBIC)	
	cv2.imshow("MT9J001",img4)
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
				time.sleep(0.5)
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


