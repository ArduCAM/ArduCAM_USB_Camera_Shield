# Overview

This is OpenCV C/C++ demo code for the AR0134 camera.
See section [Configuration File](https://github.com/ArduCAM/ArduCAM_USB_Camera_Shield/tree/master/Windows/OpenCV#configuration-file) for other camera models.

# Prerequisites

You need to use latest OpenCV3.4.1 and VS2017 Community for easy setup and JSON file support.

# Install OpenCV3.4.1 Environment

Download the package from [OpenCV3.4.1](https://sourceforge.net/projects/opencvlibrary/files/opencv-win/3.4.1/opencv-3.4.1-vc14_vc15.exe/download)

Run the self-extra .exe file to any location you preferred. You don't need to compile the source code because there is pre-compiled library for VC14/VC15.

# Install Visual Studio VS2017 Community 

Download and install the package from [VS2017](https://visualstudio.microsoft.com/thank-you-downloading-visual-studio/?sku=Community&rel=15)

# Set VS2017 Example Project Build Environment 

Project properties Pages-> VC++ Directories
Set Include Directories to your opencv\build\include;
Set Library Directories to opencv\build\x64\vc15\lib and ArduCam_test;

Project properties Pages-> Linker
Set Input-> Additional Dependencies with ArduCamlib.lib; opencv_world341d.lib; 

# Run the demo code

Compile the project and copy the opencv_world341.dll or opencv_world341d.dll from opencv\build\x64\vc15\bin to x64\Release or x64\Debug directory respectively.

This demo load the configuration file and open the camera, launch two threads one for capture and the other for display. 
In display thread it simply reads the frame and does RAW to RGB conversion using OpenCV functions. It is a simple and good start point to grab images and do some image processing.

# Configuration File

In this demo it uses JSON format to store the camera configuration, user can modify the given JSON file according to the [content](https://github.com/ArduCAM/ArduCAM_USB_Camera_Shield/tree/master/Windows/USBTest/x64/Release/Config) for other Arducam camera models.
