# Overview

Python demo code for the following cameras:

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

Download the [Python2.7](https://www.python.org/ftp/python/2.7.15/python-2.7.15.amd64.msi) or Download the [Python3.x](https://www.python.org/downloads/windows/) 

Install Python Dependence libraries 

`pip install opencv-python` or `pip3 install opencv-python`

# Usuage

`python ArduCam_Py_Demo.py <path\config-file-name>	`
example: `python ArduCam_Py_Demo.py ..\..\..\python_config\AR0134_960p_Color.json`	
While the program is running, you can press the following buttons in the terminal:	

    1.'s' + Enter:Save the image to the images folder.	
    2.'c' + Enter:Stop saving images.	
    3.'q' + Enter:Stop running the program.	
