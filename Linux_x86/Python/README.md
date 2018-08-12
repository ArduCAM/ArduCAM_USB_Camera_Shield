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

`sudo python ArduCam_Py_Demo.py <path/config-file-name>	`
example: `sudo python ArduCam_Py_Demo.py ../JSON_Config_Files/AR0134_960p_Color.json`	
While the program is running, you can press the following buttons in the terminal:	

    1.'s' + Enter:Save the image to the images folder.	
    2.'c' + Enter:Stop saving images.	
    3.'q' + Enter:Stop running the program.	
