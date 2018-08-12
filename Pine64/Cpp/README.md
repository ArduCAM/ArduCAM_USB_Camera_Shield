# Overview

This is C/C++ demo code for the following cameras:

- AR0134 (1.2MP Global Shutter Camera)

- MT9V022 Monochrome (0.36MP Global Shutter Camera)

- MT9V034 Color (0.36MP Global Shutter Camera)

- MT9M001 (1.3MP Rolling Shutter Camera)

- MT9N001 Color (9MP Rolling Shutter Camera)

- MT9J001 Monochrome (10MP Rolling Shutter Camera)

- MT9J003 Color (10MP Rolling Shutter Camera)

- MT9F002 Color (14MP Rolling Shutter Camera)

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
 example: `sudo ./ArduCam_Demo ../Config/AR0134_960p_Color.yml`
 While the program is running, you can press the following buttons in the terminal:	
 
    1.'s':Save the image to the images folder.	
    2.'c':Stop saving images.	
    3.'q':Stop running the program.	


