# Overview

This is C/C++ demo code for the following cameras:

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
## Install Python 2.7 / python-opencv and g++ compiler
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
sudo apt-get install libopencv-dev
```
- Install g++ compiler
```Bash 
sudo apt-get install g++-4.8
```

# Install the SDK
- Run the following command to install SDK
```Bash
make install-sdk
```
# Compile the source code
- Run the following command to compile the source code 
```Bash
make
```
# Usage
 `sudo ./ArduCam_Demo <path/config-file-name>`	
 
 example:
 
 ```bash
 cd ArduCAM_USB_Camera_Shield/RaspberryPi/Cpp/Streaming_demo
 ```
 ```bash
 sudo ./ArduCam_Demo ../../../cpp_config/AR0134_960p_Color.yml
 ```
 
 While the program is running, you can press the following buttons in the terminal:	
 
    1.'s':Save the image to the images folder.	
    2.'c':Stop saving images.	
    3.'q':Stop running the program.	


