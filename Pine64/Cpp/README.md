# Overview

This is ARM64 Arch C/C++ demo code for the following cameras:

- AR0134 (1.2MP Global Shutter Camera)

- MT9V022 Monochrome (0.36MP Global Shutter Camera)

- MT9V034 Color (0.36MP Global Shutter Camera)

- MT9M001 (1.3MP Rolling Shutter Camera)

- MT9N001 Color (9MP Rolling Shutter Camera)

- MT9J001 Monochrome (10MP Rolling Shutter Camera)

- MT9J003 Color (10MP Rolling Shutter Camera)

- MT9F002 Color (14MP Rolling Shutter Camera)

This demo is only demonstrate how to use the Arducam SDK functions, 

Tested under Pine64 ubuntuMATE16.04 OS Opencv 2.4.13

# Install library and OpenCV Environment

Download the [libusb](https://sourceforge.net/projects/libusb/files/libusb-1.0/) 

Unzip `tar -jxvf  libusb-1.0.21.tar.bz2`

Then configure `./configure` or `./configure --disable-udev`

Install the libusb library `sudo make install` 

Install OpenCV library `sudo apt-get install libopencv-dev`

Install g++ compiler `sudo apt-get install g++-4.8`

# Install the SDK
Copy the libArduCamLib.so to /lib folder 

# Compile the source code
Replace the xxxx with the cpp souce file name

`g++ xxxx_demo.cpp -o Test `pkg-config --cflags --libs opencv` -lArduCamLib -lusb-1.0  -lpthread -L. -I. -std=gnu++11`


# Usuage

- Realtime preview

``` sudo ./Test ```

- quit

Press `q` in the terminal window

- save image

Press `s` in the terminal window


