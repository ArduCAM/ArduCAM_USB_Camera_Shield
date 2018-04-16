# Overview

Python demo code for the following cameras:

- MT9V034 (0.36MP Global Shutter Camera)
- AR0134 (1.2MP Global Shutter Camera)
- MT9M001 (1.3MP Rolling Shutter Camera)
- MT9N001 (9MP Rolling Shutter Camera)
- MT9J001 (10MP Rolling Shutter Camera)
- MT9F002 （14MP Rolling Shutter Camera)

# Install library and OpenCV Environment

Download the [libusb](https://sourceforge.net/projects/libusb/files/libusb-1.0/) 

Unzip `tar -jxvf  libusb-1.0.21.tar.bz2`

Then configure `./configure` or `./configure --disable-udev`

Install the libusb library `sudo make install` 

Install Python 2.7 `sudo apt-get install python2.7-dev`

Install Python Dependence libraries 

`sudo apt-get install python-pip`

`sudo apt-get install python-opencv`

`sudo apt-get install python-imaging`

`sudo pip install evdev`

# Usuage

- Realtime preview

``` sudo python MT9V034 ```

``` sudo python AR0134 ```

``` sudo python MT9M001 ```

``` sudo python MT9N001 ```

``` sudo python MT9J001 ```

- Snapshot

``` sudo python MT9F002 --type jpg --name sample_image ```

## Parameters
--type : jpg, bmp, png

--name : file name without extension

## Window Control
The realtime preview mode supports zoom in/out with control the keyboard

- Zoom in key ' <- '
- Zoom out key ' -> '
- Quit key ' q '

# Troubleshooting
The keyboard event number might not match your system, using the following command to determine which is your keyboard event

``` cat /proc/bus/input/devices ```
