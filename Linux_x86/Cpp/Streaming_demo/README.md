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

Download the [libusb](https://sourceforge.net/projects/libusb/files/libusb-1.0/) 

Unzip `tar -jxvf  libusb-1.0.21.tar.bz2`

Then configure `./configure` or `./configure --disable-udev`

Install the libusb library `sudo make install` 

Install OpenCV library `sudo apt-get install libopencv-dev`

Install g++ compiler `sudo apt-get install g++-4.8`

# Install the SDK

`make install-sdk`

# Compile the source code

`make`


# Usage

 `sudo ./ArduCam_Demo <path/config-file-name>`	
 example: `sudo ./ArduCam_Demo ../../../cpp_config/AR0134_960p_Color.yml`
 While the program is running, you can press the following buttons in the terminal:	
 
    1.'s':Save the image to the images folder.	
    2.'c':Stop saving images.	
    3.'q':Stop running the program.	


