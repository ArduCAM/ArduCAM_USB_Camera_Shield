# Overview
ArduCAM USB Camera Shield is a general purpose USB camera controller board that can accommodate wide range of CMOS camera module from 0.3MP ~ 14MP.
By using provided SDK library and demo source code, user can design their own applications.
More info can be found from [arducam website](http://www.arducam.com/arducam-usb-camera-shield-released/)

## Now Supported Cameras
-	OV7670		0.3MP
-	OV7675		0.3MP
-	OV7725		0.3MP
-	MT9V034		0.36MP (global shutter)
-	MT9M112		1.3MP	
-	MT9M001		1.3MP (Monochrome/Color)	
-	AR0134		1.2MP (global shutter)
-	OV2640		2MP	
-	OV5642		5MP	
-	OV5640		5MP 
- MT9P001   5MP
-	MT9N001		9MP
-	MT9J001		10MP (Monochrome)
- MT9J003   10MP (Color)
-	MT9F002		14MP

## Support OS 
- Windows
- Linux Ubuntu
- Raspbian

## Limitations
This USB camera shield has no onboard frame buffer. The transfer reliability highly depends the USB bandwidth.  
If multiple USB devices are connected to a single USB root hub, it will cause bandwidth racing and cause drop frames.  
It is recommended to preserved enough bandwidth for the camera on USB port, or reduce the frame rate and Pixel clock of the image sensor once continuously dropping frames happens.  
This USB camera shield isn't UVC compliant, for Windows you have to install our USB driver, for Linux you can use libusb which is also plug-and-play.

### Bandwidth Calculation
The USB2.0 port bandwidth is 480Mbps, apart from the protocol overhead, the maximum available bandwidth is around 40MBytes/s.
40MBytes/s is the avarage bandwidth, due to the none realtime system on PC or ARM processor, sometimes the USB driver will be slow down at some intervals when the CPU loading changes. So the maximum stable pixel clock speed for the camera module is 24MHz.
which means 20fps for AR0134(1.2MP) at 1280x960 and 60fps for MT9V034(0.4MP) at 640x480, 1~2fps for MT9F002(14MP) tested on PC. And the fps will be slower on Raspberry pi.

### USB Driver
The USB host driver plays very important roles on image data trasfer. ArduCAM USB shield use Bulk endpoint for the image transfer, the nature of the USB bulk tranfer only promise the the correctness of the transfer,but the bandwidth is not promised. So for a given USB root hub there will be bandwidth racing between each device attached. Even if only one device is attached when some device is doing burst while the USB driver is not ready to response, the data dropping happens.

### PC
Basically the PC is fast enough to run ArduCAM USB camera shield without dropping frames.  
If you want to connect more than one ArduCAM USB camera shield to a single PC, you have to connect them seperately to the different USB root hub of the PC.

### Raspberry Pi
Raspberry Pi is far less processing power than PC, it is not fast enough to display the captured video by the ARM processor.  
And there is only one USB root hub on Raspberry Pi board, everything shares the USB bandwidth like the onboard ethernet and 4 USB ports.  
Running ArduCAM USB camera shield on Raspberry Pi might not as good as PC, It is recommended to do image processing without display the captured video in real time.  
