# Overview
This ArduCam_Ext_Trigger_Demo demostrates how to capture a single frame from the camera with external trigger input to USB2 camera shield.
The purpose of this demo is used for multi-camera synchronized capture, it works for both rolling shutter camera and global shutter camera. 
If you want to trigger multiple global shutter camera, you have set the global shutter camera into trigger mode and connect the external trigger to both camera board and USB2 camera shield.

Note that this example only works for lastest USB2 camera shield hardware.

Python demo code for the following cameras(but not limited to):

- MT9V022 (0.36MP Global Shutter Camera)
- MT9V034 (0.36MP Global Shutter Camera)
- AR0134 (1.2MP Global Shutter Camera)
- MT9N001 (9MP Rolling Shutter Camera)
- MT9J001 (10MP Rolling Shutter Camera)
- MT9J003 (10MP Rolling Shutter Camera)
- MT9F002 （14MP Rolling Shutter Camera)
- MT9F001 （14MP Rolling Shutter Camera)
- OV2640 （1.9MP Rolling Shutter Camera)
- OV5640 （5MP Rolling Shutter Camera)
- OV5642 （5MP Rolling Shutter Camera)
- OV7675 （0.36MP Rolling Shutter Camera)

# Install library and OpenCV Environment

Download the [Python2.7](https://www.python.org/ftp/python/2.7.15/python-2.7.15.amd64.msi) or Download the [Python3.x](https://www.python.org/downloads/windows/) 

Install Python Dependence libraries 

`pip install opencv-python` or `pip3 install opencv-python`

# Usuage

`python ArduCam_Ext_Trigger_Demo.py <path\config-file-name>	`
example: `python ArduCam_Ext_Trigger_Demo.py ..\..\..\python_config\AR0134_960p_Color.json`	

Press `Ctrl+C` Exit
