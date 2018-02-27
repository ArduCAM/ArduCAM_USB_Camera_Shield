# Overview

This is C/C++ demo code for the following cameras:


- AR0134 (1.2MP Global Shutter Camera)

- MT9M001 (1.3MP Rolling Shutter Camera)

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


