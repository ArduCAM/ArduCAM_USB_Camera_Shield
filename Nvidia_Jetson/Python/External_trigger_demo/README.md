# Overview

Python demo code for the following cameras:

- MT9V022 (0.36MP Global Shutter Camera)
- MT9V034 (0.36MP Global Shutter Camera)
- AR0134 (1.2MP Global Shutter Camera)
- AR0135 (1.3MP Global Shutter Camera)
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
## Build and install arducam_config_parser 
- Build  
```Bash
  git clone https://github.com/ArduCAM/arducam_config_parser.git
  cd arducam_config_parser
  make clean && make
```
- Install  
```Bash
  make install
```

## Downlaod and install the latest libusb 
- Download the [libusb](https://sourceforge.net/projects/libusb/files/libusb-1.0/) 
- Copy the libusb-xxx.tar.bz to the Pi then run the following command to unzip it.[xxx：version number]
```Bash
tar -jxvf  libusb-xxx.tar.bz2  
```
- Before compilation, Run the following commands to config it  
```Bash
cd libusb-xxx 
./configure --disable-udev
```
- Install the libusb library 
```Bash
sudo make install
```
## Install Python 2.7 and python-opencv
 Notice: On raspberry Pi platform we advise use Python 2.7 
 ```bash
 sudo apt-get install python2.7-dev
 ``` 
 - For Python3
```Bash
sudo apt-get install python3.x-dev
```
- Install opencv
```Bash
sudo apt-get install python-opencv
```

# Usuage
## For the External_trigger_demo 
```Bash 
cd ArduCAM_USB_Camera_Shield/RaspberryPi/Python/External_trigger_demo.py/
```
sudo python ArduCam_Ext_Trigger_Demo.py <path/config-file-name>
example:
```Bash
sudo python ArduCam_Ext_Trigger_Demo.py ../../../Config/USB2.0_UC-391_Rev.D/AR0134_RAW_8b_1280x964_31fps.cfg
```
Press Ctrl+C to exit the demo.


