# Overview
This Arducam_Global_Shutter_Demo demostrates how to capture a single frame from the camera with external trigger input to USB2 camera shield.
The purpose of this demo is used for multi-camera synchronized capture, it works for both rolling shutter camera and global shutter camera. 
If you want to trigger multiple global shutter camera, you have set the global shutter camera into trigger mode and connect the external trigger to both camera board and USB2 camera shield.

Note that this example only works for lastest USB2 camera shield hardware.

This is C/C++ demo code for the AR0134 (1.2MP Global Shutter Camera)
 
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
## Install Opencv and g++ compiler
- Install opencv
```Bash
sudo apt-get install libopencv-dev
```
- Install g++ compiler[the version number of g++ should more than 4.8]
```Bash 
sudo apt-get install g++-4.8
```
- Install the zbar library
```Bash
 sudo apt-get install libzbar-dev
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
 `sudo ./Arducam_Global_Shutter_Demo <path/config-file-name>`	
 
 example:
 ```bash
 cd ArduCAM_USB_Camera_Shield/RaspberryPi/Cpp/Arducam_Global_Shutter
 ```
 ```bash
 sudo ./Arducam_Global_Shutter_Demo ../../../cpp_config/AR0134_960p_Color.yml
 ```
 press Ctrl+C exit
 
# Hardware Connection
For the UC-391 Rev.C baseboard , there is not trigger signal input interface. 
So, please connect the trigger signal from the camera board. For example, 
we use AR0134 (UC-403), we can input the trigger signal as the following 
picture shows. Notice the valid trigger signal is 3.3 high level, please 
ensure your micro controller's level is 3.3v.

![Alt text]( https://github.com/ArduCAM/ArduCAM_USB_Camera_Shield/blob/master/Data/Arducam_UC-403.png)

For the UC-391 Rev.D baseboard which has external trigger signal interface, 
so you can input the trigger to the baseboard directly. Such as the following 
picture shows. Notice the valid trigger signal is 3.3 high level, please 
ensure your micro controller's level is 3.3v.

![Alt text]( https://github.com/ArduCAM/ArduCAM_USB_Camera_Shield/blob/master/Data/Arducam_UC-391_Rev_D.png)

## The test project
- Referring the video demo 
```bash
https://youtu.be/f_saNQQACOo 
```
 ![Alt text]( https://github.com/ArduCAM/ArduCAM_USB_Camera_Shield/blob/master/Data/Arducam_Global_Shutter_Shoow1.png)
 ![Alt text]( https://github.com/ArduCAM/ArduCAM_USB_Camera_Shield/blob/master/Data/Arducam_Global_Shutter_Shoow2.png)
 ![Alt text]( https://github.com/ArduCAM/ArduCAM_USB_Camera_Shield/blob/master/Data/Arducam_Global_Shutter_Shoow3.png)
 
 

 

