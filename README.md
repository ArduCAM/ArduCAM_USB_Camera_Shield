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
-	MT9M001		1.3MP 	
-	AR0134		1.2MP (global shutter)
-	OV2640		2MP	
-	OV5642		5MP	
-	OV5640		5MP 
-	MT9N001		9MP
-	MT9J001		10MP
-	MT9F002		14MP

## Support OS 
- Windows
- Linux Ubuntu

## Limitations
This USB camera shield has no onboard frame buffer. The transfer reliability highly depends the USB bandwidth.
If multiple USB devices are connected to a single USB root hub, it will cause bandwidth racing and cause drop frames.
It is recommended to preserved enough bandwidth for the camera on USB port, or reduce the frame rate and Pixel clock of the image sensor once continuously dropping frames happens.


